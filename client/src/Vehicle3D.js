import React, { useRef, useEffect } from 'react';
import { useFrame } from '@react-three/fiber';
import { useGLTF } from '@react-three/drei';

const Vehicle3D = ({ position, rotation }) => {
  const vehicleRef = useRef();
  const { nodes, materials } = useGLTF('./Vehicle3d/carro/scene.gltf'); // Substitua pelo caminho do seu modelo 3D

  useFrame(() => {
    if (vehicleRef.current) {
      vehicleRef.current.position.set(position[0], position[1], position[2]);
      vehicleRef.current.rotation.set(rotation[0], rotation[1], rotation[2]);
    }
  });

  return (
    <group ref={vehicleRef} dispose={null}>
      <mesh geometry={nodes.YourVehicleMesh.geometry} material={materials.YourVehicleMaterial} />
    </group>
  );
};

export default Vehicle3D;
