from fastapi import FastAPI, HTTPException, Request
import json
from pydantic import BaseModel
from fastapi.encoders import jsonable_encoder
import base64
import os
from datetime import datetime
from fastapi.responses import JSONResponse
import paho.mqtt.client as mqtt

class Device(BaseModel):
    id: int
    value: int

class ESP(BaseModel):
    hashcode: str
    states: list[Device]
app = FastAPI()
# database
ESP_FILE = "esp.json"
ESP_DATABASE : list[ESP] = []
HEALTH_FILE = "health_check.json"

#mqtt config
MQTT_BROKER = "anhpn.ddns.net"
MQTT_PORT = 1884
mqtt_client = mqtt.Client()
MQTT_USERNAME = "ductran"
MQTT_PASSWORD = "Duc@2025"
mqtt_client.username_pw_set(MQTT_USERNAME, MQTT_PASSWORD)
mqtt_client.connect(MQTT_BROKER, MQTT_PORT, 60)
mqtt_client.loop_start()

DEVICE_LIMITS = 4
if os.path.exists(ESP_FILE):
    try:
        with open(ESP_FILE, "r") as f:
            # Load the data if the file exists
            ESP_DATABASE = json.load(f)
    except json.JSONDecodeError:
        # Handle case where file might be corrupted/empty
        print(f"Warning: {ESP_FILE} exists but is empty or corrupt. Using initial data.")


def encode_mac(mac: bytes) -> str:
    if len(mac) != 6:
        raise HTTPException(400, f"invalid mac {mac}")
    encoded = base64.b64encode(mac).decode('ascii')
    return encoded

def api_subscribe(topic: str):
    for x in range(DEVICE_LIMITS):
        mqtt_client.subscribe(f"{topic}/device_{x}/set")

@app.post("/get-hash-code")
async def get_mac(esp: ESP):
    esp.hashcode = encode_mac(bytes.fromhex(esp.hashcode.replace(":", "")))
    for x in ESP_DATABASE:
        if x["hashcode"] == esp.hashcode:
            return {"message" : f"mac {x} was available", "hashcode" : esp.hashcode}
    api_subscribe(esp.hashcode)
    json_esp = jsonable_encoder(esp)
    ESP_DATABASE.append(json_esp)
    with open(ESP_FILE, "w") as fa:
        json.dump(ESP_DATABASE, fa, indent=4)
    return {"message" : f"mac {esp.hashcode} was added", "hashcode" : esp.hashcode}

@app.get("/get-state/{hashcode}")
async def get_state(hashcode : str):
    for x in ESP_DATABASE:
        if x["hashcode"] == hashcode:
            return x
    raise HTTPException(404, f"hashcode {hashcode} not found")

def on_mqtt_message(client, userdata, message):
    payload = message.payload.decode("utf-8")
    for x in ESP_DATABASE:
        for y in x.states:
            topic = f"{x.hashcode}/device_{y.id}/set"
            if topic == message.topic:
                y.value = int(payload)
    # Save back after change
    with open(ESP_FILE, "w") as fa:
        json.dump([jsonable_encoder(esp) for esp in ESP_DATABASE], fa, indent=4)

@app.post("/set-state")
async def set_state(esp: ESP):
    # Find target ESP in database
    for x in ESP_DATABASE:
        if x["hashcode"] == esp.hashcode:
            # Update matching states
            for new_state in esp.states:
                for y in x["states"]:
                    if y["id"] == new_state.id:
                        y["value"] = new_state.value
                        break

            # Save updated database before publish
            with open(ESP_FILE, "w") as fa:
                json.dump(ESP_DATABASE, fa, indent=4)

            # Publish updated states (use updated value)
            for y in x["states"]:
                topic = f"{esp.hashcode}/device_{y['id']}/set"
                mqtt_client.publish(topic, str(y["value"]))
                print(f"Published to {topic}: {y['value']}")

            return {"message": "State updated successfully"}

    raise HTTPException(404, f"Hashcode {esp.hashcode} not found")

@app.post("/health-check")
async def health_check(request: Request):
    try:
        # Parse incoming JSON from ESP32
        data = await request.json()
        print("Received health data:", data)

        # Add timestamp
        data["timestamp"] = datetime.now().strftime("%Y-%m-%d %H:%M:%S")

        # Load existing file
        if os.path.exists(HEALTH_FILE):
            with open(HEALTH_FILE, "r") as fa:
                try:
                    health_data = json.load(fa)
                except json.JSONDecodeError:
                    health_data = []
        else:
            health_data = []

        # Append new entry
        health_data.append(data)

        # Save updated list back to file
        with open(HEALTH_FILE, "w") as fb:
            json.dump(health_data, fb, indent=4)

        return JSONResponse(
            content={"message": "Health check successfully"},
            status_code=200
        )

    except Exception as e:
        return JSONResponse(
            content={"error": str(e)},
            status_code=500
        )
