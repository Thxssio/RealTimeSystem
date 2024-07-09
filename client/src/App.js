import React, { useEffect, useState, useRef } from 'react';
import { MapContainer, TileLayer, Marker, Popup } from 'react-leaflet';
import L from 'leaflet';
import 'leaflet/dist/leaflet.css';
import './styles.css';

const SERVER_IP = 'http://192.168.0.85:8888';

// Cria o ícone personalizado usando CSS
const createCustomIcon = (status) => {
  return L.divIcon({
    className: `marker ${status === 'disconnected' ? 'disconnected' : 'connected'}`,
    iconSize: [20, 20],
    iconAnchor: [10, 10],
    popupAnchor: [0, -10],
  });
};

const App = () => {
  const [gpsData, setGpsData] = useState(null);
  const [error, setError] = useState(null);
  const [loading, setLoading] = useState(true);
  const [command, setCommand] = useState('');
  const mapRef = useRef();
  const hasCenteredMap = useRef(false);

  const isZeroedData = (data) => {
    return data.latitude === 0 && data.longitude === 0 && data.altitude === 0 && data.quality === 0 && data.satellites === 0;
  };

  useEffect(() => {
    const fetchGpsData = async () => {
      try {
        const response = await fetch(`${SERVER_IP}/gps-data`);
        if (!response.ok) {
          throw new Error(`Erro HTTP! status: ${response.status}`);
        }
        const data = await response.json();
        setGpsData(data);

        if (isZeroedData(data)) {
          console.error("Dados GPS zerados recebidos:", data);
          setLoading(true);
        } else {
          console.log("Dados GPS recebidos:", data);
          setLoading(false);
        }
      } catch (error) {
        console.error("Falha ao buscar dados GPS:", error);
        setError(error.toString());
        setLoading(true);
      }
    };

    const interval = setInterval(fetchGpsData, 1000);

    return () => clearInterval(interval);
  }, []);

  useEffect(() => {
    if (gpsData && !isZeroedData(gpsData) && mapRef.current && !hasCenteredMap.current) {
      const { latitude, longitude } = gpsData;
      mapRef.current.setView([latitude, longitude], 13);
      hasCenteredMap.current = true;
    }
  }, [gpsData]);

  const handleCommandSubmit = async (e) => {
    e.preventDefault();
    console.log('Comando enviado:', command); // Log do comando enviado

    const commandData = { command };
    console.log('Dados do comando enviados:', JSON.stringify(commandData)); // Log dos dados enviados

    try {
        const response = await fetch(`${SERVER_IP}/send-command`, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify(commandData),
        });

        console.log('Requisição enviada:', response); // Log da requisição enviada

        if (!response.ok) {
            throw new Error(`Erro ao enviar comando! status: ${response.status}`);
        }

        const data = await response.json();
        console.log('Resposta do servidor:', data);

    } catch (error) {
        console.error('Falha ao enviar comando:', error);
    }
};

  const bounds = [
    [-85, -180],
    [85, 180]
  ];

  return (
    <div className="App">
      <h1>GPS Tracker</h1>
      <form className="command-form" onSubmit={handleCommandSubmit}>
        <input
          type="text"
          value={command}
          onChange={(e) => setCommand(e.target.value)}
          placeholder="Digite o comando"
        />
        <button type="submit">Enviar Comando</button>
      </form>
      {error && <p style={{ color: 'red' }}>{error}</p>}
      <MapContainer
        ref={mapRef}
        center={[0, 0]}
        zoom={10}
        minZoom={2}
        maxZoom={18}
        maxBounds={bounds}
        maxBoundsViscosity={1.0}
        style={{
          height: '100vh',
          width: '100vw',
          position: 'absolute',
          top: 0,
          left: 0,
        }}
      >
        <TileLayer
          url="https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png"
          attribution='&copy; <a href="https://www.openstreetmap.org/copyright">OpenStreetMap</a> contributors'
        />
        {(loading || (gpsData && isZeroedData(gpsData))) && (
          <div className="loading-overlay">
            <div className="loading-spinner"></div>
            <p>Carregando dados do GPS...</p>
          </div>
        )}
        {gpsData && !isZeroedData(gpsData) && (
          <Marker position={[gpsData.latitude, gpsData.longitude]} icon={createCustomIcon(gpsData.status)}>
            <Popup>
              <div className="popup-content">
                <div>
                  <p>Latitude: {gpsData.latitude}</p>
                  <p>Longitude: {gpsData.longitude}</p>
                  <p>Altitude: {gpsData.altitude}m</p>
                  <p>Status: {gpsData.status}</p>
                  <p>Satélites: {gpsData.satellites}</p>
                  <p>Qualidade: {gpsData.quality}</p>
                </div>
                <div>
                  <h2>Informações dos Satélites:</h2>
                  <ul className="satellite-info">
                    {gpsData.satellites_info.map((sat, index) => (
                      <li key={index}>
                        PRN: {sat.prn}, Elevação: {sat.elevation}, Azimute: {sat.azimuth}, SNR: {sat.snr}
                      </li>
                    ))}
                  </ul>
                </div>
                <div>
                  <h2>Dados do MPU9250:</h2>
                  <ul className="mpu9250-info">
                    <li>Aceleração X: {gpsData.accel_x}</li>
                    <li>Aceleração Y: {gpsData.accel_y}</li>
                    <li>Aceleração Z: {gpsData.accel_z}</li>
                    <li>Giroscópio X: {gpsData.gyro_x}</li>
                    <li>Giroscópio Y: {gpsData.gyro_y}</li>
                    <li>Giroscópio Z: {gpsData.gyro_z}</li>
                    <li>Magnetômetro X: {gpsData.mag_x}</li>
                    <li>Magnetômetro Y: {gpsData.mag_y}</li>
                    <li>Magnetômetro Z: {gpsData.mag_z}</li>
                  </ul>
                </div>
              </div>
            </Popup>
          </Marker>
        )}
        {gpsData && gpsData.status === 'disconnected' && !isZeroedData(gpsData) && (
          <div className="disconnected-overlay">
            <p1>GPS Desconectado. Exibindo última localização conhecida.</p1>
          </div>
        )}
      </MapContainer>
    </div>
  );
};

export default App;
