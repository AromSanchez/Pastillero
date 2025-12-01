from pastillas.models import ConfiguracionArduino

# Verificar configuración
config = ConfiguracionArduino.objects.first()

if config:
    print(f"Configuración encontrada:")
    print(f"  IP Arduino: {config.ip_arduino}")
    print(f"  Puerto: {config.puerto}")
    print(f"  Activo: {config.activo}")
else:
    print("No hay configuración de Arduino")
    print("Creando configuración por defecto...")
    config = ConfiguracionArduino.objects.create(
        ip_arduino="10.147.1.1",
        puerto=80,
        activo=True
    )
    print(f"✅ Configuración creada: {config}")
