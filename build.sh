#!/bin/bash
set -e

echo "🔧 Build script para Render iniciado..."

# Verificar que estamos en el directorio correcto
pwd
ls -la

# Instalar dependencias si no existen
if [ ! -d "node_modules" ]; then
    echo "📦 Instalando dependencias..."
    npm ci
else
    echo "✅ Dependencies already installed"
fi

# Verificar que playwright está instalado y configurado
echo "🎭 Verificando Playwright..."
if command -v npx &> /dev/null; then
    echo "📦 Instalando Chromium para Playwright..."
    npx playwright install chromium --with-deps || echo "⚠️ Playwright install failed, continuing..."
else
    echo "⚠️ npx no disponible, saltando Playwright install"
fi

# Verificar que glob existe
if [ ! -d "node_modules/glob" ]; then
    echo "❌ Glob missing, installing..."
    npm install glob
fi

# Crear directorio dist
mkdir -p dist

# Ejecutar la generación
echo "📚 Generando notebook..."
node src/generate.js

echo "✅ Build completado exitosamente!"
ls -la dist/
