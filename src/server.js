const express = require('express');
const path = require('path');
const fs = require('fs');

const app = express();
const DIST = path.join(process.cwd(), 'dist');

// Middleware para servir archivos estáticos
app.use('/dist', express.static(DIST));
app.use(express.static('public'));

// Verificar si los archivos existen antes de servir
app.get('/', (req, res) => {
  const htmlExists = fs.existsSync(path.join(DIST, 'notebook.html'));
  const pdfExists = fs.existsSync(path.join(DIST, 'notebook.pdf'));
  
  res.send(`
    <html>
      <head>
        <meta charset="utf-8" />
        <title>Testigos de Turing - Notebook ICPC</title>
        <style>
          body{font-family:system-ui;padding:24px;background:#f5f5f5}
          .container{max-width:800px;margin:0 auto;background:white;padding:32px;border-radius:12px;box-shadow:0 4px 6px rgba(0,0,0,0.1)}
          iframe{width:100%;height:80vh;border:1px solid #ddd;border-radius:12px}
          .bar{display:flex;gap:12px;margin-bottom:12px;flex-wrap:wrap}
          a.button{padding:12px 24px;border-radius:8px;background:#2563eb;color:#fff;text-decoration:none;font-weight:500;transition:background 0.2s}
          a.button:hover{background:#1d4ed8}
          a.button:disabled{background:#9ca3af;cursor:not-allowed}
          .status{padding:8px 12px;border-radius:6px;margin-bottom:16px}
          .success{background:#dcfce7;color:#166534;border:1px solid #bbf7d0}
          .error{background:#fef2f2;color:#dc2626;border:1px solid #fecaca}
          .header{text-align:center;margin-bottom:24px}
          .header h1{color:#1f2937;margin-bottom:8px}
          .header p{color:#6b7280}
        </style>
      </head>
      <body>
        <div class="container">
          <div class="header">
            <h1>🏆 Testigos de Turing - Notebook ICPC</h1>
            <p>Compendio de algoritmos para competencias de programación</p>
          </div>
          
          ${!htmlExists && !pdfExists ? `
            <div class="status error">
              ⚠️ Los archivos del notebook no se han generado aún. Esto puede tomar unos minutos en el primer deploy.
            </div>
          ` : `
            <div class="status success">
              ✅ Notebook generado correctamente
            </div>
          `}
          
          <div class="bar">
            ${pdfExists ? `
              <a class="button" href="/dist/notebook.pdf" download>📄 Descargar PDF</a>
            ` : `
              <span class="button" style="background:#9ca3af;cursor:not-allowed">📄 PDF (Generando...)</span>
            `}
            ${htmlExists ? `
              <a class="button" href="/dist/notebook.html" target="_blank">🌐 Ver HTML</a>
            ` : `
              <span class="button" style="background:#9ca3af;cursor:not-allowed">🌐 HTML (Generando...)</span>
            `}
            <a class="button" href="https://github.com/IvanTicona/TestigosDeTuring-Notebook" target="_blank" style="background:#24292f">💻 Ver Código</a>
          </div>
          
          ${htmlExists ? `
            <iframe src="/dist/notebook.html"></iframe>
          ` : `
            <div style="display:flex;align-items:center;justify-content:center;height:400px;background:#f9fafb;border-radius:8px;color:#6b7280">
              <div style="text-align:center">
                <div style="font-size:48px;margin-bottom:16px">⏳</div>
                <p>Generando notebook...</p>
                <p style="font-size:14px">Recarga la página en unos minutos</p>
              </div>
            </div>
          `}
        </div>
      </body>
    </html>
  `);
});

// Ruta de salud para Render
app.get('/health', (req, res) => {
  res.json({ status: 'ok', timestamp: new Date().toISOString() });
});

const PORT = process.env.PORT || 10000; // Render usa puerto 10000 por defecto
app.listen(PORT, '0.0.0.0', () => {
  console.log(`🚀 Servidor corriendo en puerto ${PORT}`);
  console.log(`📁 Directorio dist: ${DIST}`);
  console.log(`📊 Archivos disponibles:`, fs.readdirSync(DIST).length > 0 ? fs.readdirSync(DIST) : 'Ninguno');
});
