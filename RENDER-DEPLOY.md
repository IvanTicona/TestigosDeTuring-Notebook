# 🚀 Deploy en Render - Instrucciones

## Configuración Recomendada en Render

### Build Settings:
- **Build Command:** `npm ci && npm run build`
- **Start Command:** `npm start`
- **Node Version:** >=18.0.0 (detectado automáticamente)

### Variables de Entorno:
```
NODE_ENV=production
```

## 📋 Scripts Disponibles

- `npm run build` - Build principal (genera HTML + PDF si es posible)
- `npm run build:simple` - Build alternativo (misma lógica, usa generate-safe.js)
- `npm start` - Inicia el servidor web

## 🔧 Solución de Problemas

### Si el build falla:

1. **Cambiar Build Command a:** `npm ci && npm run build:simple`
2. **O usar fallback completo:** `npm ci && mkdir -p dist && node src/generate-safe.js`

### Logs importantes a buscar:

✅ **Éxito:**
```
✅ Playwright disponible
✅ Highlighter inicializado
HTML: /path/to/dist/notebook.html
PDF: /path/to/dist/notebook.pdf
```

⚠️ **Fallback (aceptable):**
```
⚠️ Playwright no disponible, modo solo HTML
HTML: /path/to/dist/notebook.html
PDF: No generado (Playwright no disponible)
```

## 🎯 URLs después del deploy:

- `/` - Vista principal del notebook
- `/dist/notebook.html` - Notebook en HTML
- `/dist/notebook.pdf` - PDF (si se generó)
- `/health` - Endpoint de salud

## 📁 Archivos importantes:

- `src/generate-safe.js` - Generador con fallback robusto
- `src/server.js` - Servidor web con manejo de errores
- `package.json` - Todas las dependencias en el lugar correcto
