import React, { useEffect, useState, useRef } from 'react';
import { MapContainer, TileLayer, Marker, Popup } from 'react-leaflet';
import 'leaflet/dist/leaflet.css';
import './styles.css';

const App = () => {
  const [gpsData, setGpsData] = useState(null);
  const [error, setError] = useState(null);
  const mapRef = useRef();

  const isValidCoordinate = (latitude, longitude) => {
    return latitude >= -90 && latitude <= 90 && longitude >= -180 && longitude <= 180;
  };

  useEffect(() => {
    const fetchGpsData = async () => {
      try {
        const response = await fetch('http://localhost:5000/gps-data');
        if (!response.ok) {
          throw new Error(`HTTP error! status: ${response.status}`);
        }
        const data = await response.json();
        if (isValidCoordinate(data.latitude, data.longitude)) {
          console.log("Received GPS data:", data);
          setGpsData(data);
        } else {
          console.error("Invalid GPS coordinates received:", data);
        }
      } catch (error) {
        console.error("Failed to fetch GPS data:", error);
        setError(error.toString());
      }
    };

    const interval = setInterval(fetchGpsData, 1000);

    return () => clearInterval(interval);
  }, []);

  useEffect(() => {
    if (gpsData && mapRef.current) {
      const { latitude, longitude } = gpsData;
      mapRef.current.setView([latitude, longitude], 13);
    }
  }, [gpsData]);

  return (
    <div className="App">
      <h1>GPS Tracker</h1>
      {error && <p style={{ color: 'red' }}>{error}</p>}
      {gpsData ? (
        <MapContainer ref={mapRef} center={[gpsData.latitude, gpsData.longitude]} zoom={13} style={{ height: '400px' }}>
          <TileLayer
            url="https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png"
            attribution='&copy; <a href="https://www.openstreetmap.org/copyright">OpenStreetMap</a> contributors'
          />
          <Marker position={[gpsData.latitude, gpsData.longitude]}>
            <Popup>
              Latitude: {gpsData.latitude}, Longitude: {gpsData.longitude}, Altitude: {gpsData.altitude}m
            </Popup>
          </Marker>
        </MapContainer>
      ) : (
        <p>Obtendo dados do GPS...</p>
      )}
    </div>
  );
};

export default App;
