#!/usr/bin/env python3
"""
Script de Prueba: Backend Django del Pastillero
Validar endpoints de la API REST
"""

import requests
import json
from datetime import datetime

BASE_URL = "http://10.147.1.123:8000/api"

def test_obtener_tratamientos():
    print("\n=== TEST: GET /api/tratamientos/ ===")
    response = requests.get(f"{BASE_URL}/tratamientos/")
    print(f"Status: {response.status_code}")
    if response.status_code == 200:
        data = response.json()
        print(f"Tratamientos: {len(data)}")
        return True, data
    return False, []

def test_crear_tratamiento():
    print("\n=== TEST: POST /api/tratamientos/ ===")
    nuevo = {
        "compartimento": 1,
        "nombre_pastilla": "Test",
        "dosis": 1,
        "stock": 10,
        "repeticion": "DIARIO",
        "hora_toma": "14:00:00",
        "fecha_inicio": datetime.now().strftime("%Y-%m-%d"),
        "activo": True
    }
    response = requests.post(f"{BASE_URL}/tratamientos/", json=nuevo)
    print(f"Status: {response.status_code}")
    if response.status_code == 201:
        data = response.json()
        print(f"ID creado: {data.get('id')}")
        return True, data.get('id')
    return False, None

def test_registrar_toma(tratamiento_id, estado="TOMADA"):
    print(f"\n=== TEST: POST /api/registrar-toma/ ({estado}) ===")
    datos = {"tratamiento_id": tratamiento_id, "estado": estado}
    response = requests.post(f"{BASE_URL}/registrar-toma/", json=datos)
    print(f"Status: {response.status_code}")
    return response.status_code == 201

def test_historial():
    print("\n=== TEST: GET /api/historial/ ===")
    response = requests.get(f"{BASE_URL}/historial/")
    print(f"Status: {response.status_code}")
    if response.status_code == 200:
        data = response.json()
        print(f"Registros: {len(data)}")
        return True
    return False

def main():
    print("="*50)
    print("SUITE DE PRUEBAS - BACKEND PASTILLERO")
    print("="*50)
    
    resultados = {"total": 0, "exitosas": 0}
    
    # Test 1
    resultados["total"] += 1
    if test_obtener_tratamientos()[0]:
        resultados["exitosas"] += 1
    
    # Test 2
    resultados["total"] += 1
    exito, tid = test_crear_tratamiento()
    if exito:
        resultados["exitosas"] += 1
    
    if tid:
        # Test 3
        resultados["total"] += 1
        if test_registrar_toma(tid, "TOMADA"):
            resultados["exitosas"] += 1
        
        # Test 4
        resultados["total"] += 1
        if test_registrar_toma(tid, "OMITIDA"):
            resultados["exitosas"] += 1
    
    # Test 5
    resultados["total"] += 1
    if test_historial():
        resultados["exitosas"] += 1
    
    print("\n" + "="*50)
    print(f"RESULTADOS: {resultados['exitosas']}/{resultados['total']} exitosas")
    print(f"Tasa de éxito: {(resultados['exitosas']/resultados['total'])*100:.1f}%")

if __name__ == "__main__":
    main()
