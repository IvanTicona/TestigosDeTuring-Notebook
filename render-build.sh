#!/bin/bash

echo "🔧 Iniciando build para Render..."

# Instalar dependencias
echo "📦 Instalando dependencias..."
npm ci

# Instalar Playwright con dependencias del sistema
echo "🎭 Instalando Playwright..."
npx playwright install-deps
npx playwright install chromium

# Crear directorio dist si no existe
mkdir -p dist

# Generar notebook
echo "📚 Generando notebook..."
npm run build

echo "✅ Build completado!"
echo "📁 Archivos generados:"
ls -la dist/
