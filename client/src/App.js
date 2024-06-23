import React, { useEffect, useState, useRef } from 'react';
import { MapContainer, TileLayer, Marker, Popup } from 'react-leaflet';
import L from 'leaflet';
import 'leaflet/dist/leaflet.css';
import './styles.css';

const SERVER_IP = process.env.REACT_APP_SERVER_IP || 'http://localhost:8888';

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

  // Definindo limites para a área visualizável no mapa
  const bounds = [
    [-85, -180], // sudoeste
    [85, 180]    // nordeste
  ];

  return (
    <div className="App">
      <h1>GPS Tracker</h1>
      {error && <p style={{ color: 'red' }}>{error}</p>}
      <MapContainer ref={mapRef} center={[0, 0]} zoom={10} minZoom={2} maxZoom={18} maxBounds={bounds} maxBoundsViscosity={1.0} style={{ height: '100vh', width: '100vw', position: 'absolute', top: 0, left: 0 }}>
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
              <div>
                <p1>Latitude: {gpsData.latitude}</p1>
              </div>
              <div>
                <p1>Longitude: {gpsData.longitude}</p1>
              </div>
              <div>
                <p1>Altitude: {gpsData.altitude}m</p1>
              </div>
              <div>
                <p1>Status: {gpsData.status}</p1>
              </div>
            </Popup>
          </Marker>
        )}
        {gpsData && gpsData.status === 'disconnected' && !isZeroedData(gpsData) && (
          <div className="disconnected-overlay">
            <p>GPS Desconectado. Exibindo última localização conhecida.</p>
          </div>
        )}
      </MapContainer>
    </div>
  );
};

export default App;
