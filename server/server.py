from fastapi import FastAPI
from fastapi.middleware.cors import CORSMiddleware
from pydantic import BaseModel
import random

app = FastAPI()

# Configuração de CORS
app.add_middleware(
    CORSMiddleware,
    allow_origins=["http://localhost:3000"],  # Permitir a origem do cliente React
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

class GPSData(BaseModel):
    latitude: float
    longitude: float
    altitude: float

@app.get("/gps-data", response_model=GPSData)
def get_gps_data():
    # Simular dados GPS aleatórios para fins de demonstração
    return GPSData(
        latitude=random.uniform(-90, 90),
        longitude=random.uniform(-180, 180),
        altitude=random.uniform(0, 1000)
    )

if __name__ == "__main__":
    import uvicorn
    uvicorn.run(app, host="0.0.0.0", port=5000)
