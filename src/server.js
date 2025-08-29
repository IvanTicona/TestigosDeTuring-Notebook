const express = require('express');
const path = require('path');

const app = express();
const DIST = path.join(process.cwd(), 'dist');

app.use('/dist', express.static(DIST));

app.get('/', (req, res) => {
  res.send(`
    <html>
      <head>
        <meta charset="utf-8" />
        <title>Algonotes – Preview</title>
        <style>
          body{font-family:system-ui;padding:24px}
          iframe{width:100%;height:80vh;border:1px solid #ddd;border-radius:12px}
          .bar{display:flex;gap:12px;margin-bottom:12px}
          a.button{padding:8px 12px;border-radius:8px;background:#111;color:#fff;text-decoration:none}
        </style>
      </head>
      <body>
        <h1>Algonotes – Preview</h1>
        <div class="bar">
          <a class="button" href="/dist/notebook.pdf" download>Descargar PDF</a>
          <a class="button" href="/dist/notebook.html" target="_blank">Ver HTML</a>
        </div>
        <iframe src="/dist/notebook.html"></iframe>
      </body>
    </html>
  `);
});

const PORT = process.env.PORT || 51010;
app.listen(PORT, () => {
  console.log('Preview en http://localhost:' + PORT);
});
