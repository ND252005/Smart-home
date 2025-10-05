from uuid import uuid4
from fastapi import FastAPI, HTTPException
import random
import os
import json
from pydantic import BaseModel
from typing import Optional
from fastapi.encoders import jsonable_encoder
import base64
app = FastAPI()

#device model
class Device(BaseModel):
    name: str
    status: bool
    device_id: Optional[str] = uuid4().hex

class esp(BaseModel):
    esp_id = str

DEVICE_FILE = "devices.json"
DEVICE_DATABASE = []

if os.path.exists(DEVICE_FILE):
    try:
        with open(DEVICE_FILE, "r") as f:
            # Load the data if the file exists
            DEVICE_DATABASE = json.load(f)
    except json.JSONDecodeError:
        # Handle case where file might be corrupted/empty
        print(f"Warning: {DEVICE_FILE} exists but is empty or corrupt. Using initial data.")

@app.get("/")
async def home():
    return {"message": "device alive"}

@app.get("/list-devices")
async def list_devices():
    return {"esp32" : DEVICE_DATABASE}

@app.get("/device-index/{index}")
async def device_index(index: int):
    if index < 0 or index > len(DEVICE_DATABASE):
        raise HTTPException(404, f"index {index} is out of range {len(DEVICE_DATABASE)}")
    else:
        return {"device" : DEVICE_DATABASE[index]}
@app.get("/get-random-device")
async def get_random_device():
    return random.choice(DEVICE_DATABASE)

@app.post("/add-device")
async def add_device(device: Device):
    device.device_id = uuid4().hex
    json_device = jsonable_encoder(device)
    DEVICE_DATABASE.append(json_device)
    with open(DEVICE_FILE, "w") as fa:
        json.dump(DEVICE_DATABASE, fa)
    return {"message" : f"device {device} was added", "device_id" : device.device_id}
@app.get("/get-device")
async def get_device(device_id: str):
    for device in DEVICE_DATABASE:
        if device["device_id"] == device_id:
            return device

    raise HTTPException(404, f"{device_id} is not found")

def encode_mac(mac: bytes) -> str:
    if len(mac) != 6:
        raise HTTPException(400, f"invalid mac {mac}")
    encoded = base64.b64encode(mac).decode('ascii')
    return encoded
@app.get('/get-hash-code')
async def get_mac(mac: str):
    mac = encode_mac(mac)
