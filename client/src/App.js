import React, { useEffect, useState, useRef } from 'react';
import { MapContainer, TileLayer, Marker, Popup } from 'react-leaflet';
import L from 'leaflet';
import 'leaflet/dist/leaflet.css';
import './styles.css';

const SERVER_IP = process.env.REACT_APP_SERVER_IP || 'http://localhost:8888';

// Cria o ícone personalizado usando CSS
const createCustomIcon = () => {
  return L.divIcon({
    className: 'marker',
    html: '<div class="marker"></div>',
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

  const isValidCoordinate = (latitude, longitude) => {
    return latitude !== 0 && longitude !== 0 && latitude >= -90 && latitude <= 90 && longitude >= -180 && longitude <= 180;
  };

  useEffect(() => {
    const fetchGpsData = async () => {
      try {
        const response = await fetch(`${SERVER_IP}/gps-data`);
        if (!response.ok) {
          throw new Error(`HTTP error! status: ${response.status}`);
        }
        const data = await response.json();
        if (isValidCoordinate(data.latitude, data.longitude)) {
          console.log("Received GPS data:", data);
          setGpsData(data);
          setLoading(false);
        } else {
          console.error("Invalid GPS coordinates received:", data);
          setLoading(true);
        }
      } catch (error) {
        console.error("Failed to fetch GPS data:", error);
        setError(error.toString());
        setLoading(true);
      }
    };

    const interval = setInterval(fetchGpsData, 1000);

    return () => clearInterval(interval);
  }, []);

  useEffect(() => {
    if (gpsData && mapRef.current && !hasCenteredMap.current) {
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
        {loading ? (
          <div className="loading-overlay">
            <div className="loading-spinner"></div>
            <p>Carregando dados do GPS...</p>
          </div>
        ) : (
          gpsData && (
            <Marker position={[gpsData.latitude, gpsData.longitude]} icon={createCustomIcon()}>
              <Popup>
                Latitude: {gpsData.latitude}, Longitude: {gpsData.longitude}, Altitude: {gpsData.altitude}m
              </Popup>
            </Marker>
          )
        )}
      </MapContainer>
    </div>
  );
};

export default App;
