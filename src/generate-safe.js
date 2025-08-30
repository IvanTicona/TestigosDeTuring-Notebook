const fs = require('fs');
const fsp = fs.promises;
const path = require('path');
const glob = require('glob');
const Handlebars = require('handlebars');
const matter = require('gray-matter');

// Verificar si Playwright está disponible
let createHighlighter, chromium;
let playwrightAvailable = false;

try {
  const shiki = require('shiki');
  createHighlighter = shiki.createHighlighter;
  const playwright = require('playwright');
  chromium = playwright.chromium;
  playwrightAvailable = true;
  console.log('✅ Playwright disponible');
} catch (error) {
  console.log('⚠️ Playwright no disponible, modo solo HTML:', error.message);
  playwrightAvailable = false;
}

const ROOT = process.cwd();
const SRC_DIRS = ['algorithms'];
const DIST_DIR = path.join(ROOT, 'dist');
const TEMPLATE_PATH = path.join(ROOT, 'src', 'template.hbs');
const CSS_PATH = path.join(ROOT, 'public', 'styles.css');

function pickLanguageByExt(file) {
  const ext = path.extname(file).toLowerCase();
  if (ext === '.cpp' || ext === '.cc' || ext === '.cxx') return 'cpp';
  if (ext === '.hpp' || ext === '.hh' || ext === '.hxx') return 'cpp';
  if (ext === '.h') return 'cpp'; // la mayoría serán C/C++
  return 'text';
}

async function collectFiles() {
  const patterns = SRC_DIRS.map(d => path.join(d, '**/*.{cpp,cc,cxx,h,hpp,hh,hxx}'));
  
  console.log('[DEBUG] CWD:', process.cwd());
  console.log('[DEBUG] SRC_DIRS:', SRC_DIRS);
  console.log('[DEBUG] GLOB patterns:', patterns);

  const pattern = patterns.length === 1 ? patterns[0] : `{${patterns.join(',')}}`;

  const files = glob.sync(pattern, {
    ignore: ['**/node_modules/**', '**/dist/**', '**/.git/**'],
    nodir: true,
    windowsPathsNoEscape: true, // importante en Windows
    nocase: true                // .CPP/.H en mayúsculas también matchea
  });  
  
  console.log('[DEBUG] Matched files:', files.length);
  if (files.length) {
    console.log('[DEBUG] Ejemplo:', files.slice(0, 5));
  } else {
    console.log('[HINT] No se encontró nada. ¿Tus .cpp están realmente en "algorithms/"?');
    console.log('[HINT] Revisa también la extensión (.cpp/.hpp) y que SRC_DIRS == ["algorithms"].');
  }

  files.sort();
  return files;
}

function categoryFromPath(file) {
  const parts = file.replace(/\\/g, '/').split('/');
  const idx = parts.indexOf('algorithms'); // cambia si usas otra raíz
  if (idx >= 0) {
    const segs = parts.slice(idx + 1, parts.length - 1); // carpetas después de algorithms
    if (segs.length) return segs.join(' / ');
  }
  // si no está bajo algorithms/, usa la carpeta inmediata
  const dir = path.dirname(file).split(/[\/\\]/).pop();
  return dir || null;
}

function stripBOM(s) {
  return s.charCodeAt(0) === 0xFEFF ? s.slice(1) : s;
}

function extractFrontmatter(content, filePath = '') {
  const src = stripBOM(content);

  // Busca front-matter en tres formatos, anclado al inicio (permitiendo espacios)
  const patterns = [
    {
      kind: 'block-comment',
      re: /^\s*\/\*\s*---\s*\r?\n([\s\S]*?)\r?\n\s*---\s*\*\/\s*/ // /* --- ... --- */
    },
    {
      kind: 'line-comment',
      re: /^\s*\/\/\s*---\s*\r?\n([\s\S]*?)\r?\n\s*\/\/\s*---\s*\r?\n?/ // // --- ... // ---
    },
    {
      kind: 'plain-yaml',
      re: /^\s*---\s*\r?\n([\s\S]*?)\r?\n---\s*\r?\n?/ // --- ... ---
    }
  ];

  for (const p of patterns) {
    const m = src.match(p.re);
    if (m) {
      const yaml = m[1];
      try {
        const rebuilt = `---\n${yaml}\n---\n` + src.slice(m[0].length);
        const parsed = matter(rebuilt);
        console.log(`[DEBUG] front-matter (${p.kind}) ok →`, Object.keys(parsed.data));
        return { data: parsed.data || {}, body: src.slice(m[0].length) };
      } catch (e) {
        console.warn(`[WARN] front-matter (${p.kind}) falló en ${filePath}:`, e.message);
      }
    }
  }

  // Sin front-matter
  return { data: {}, body: src };
}

async function buildHtml() {
  await fsp.mkdir(DIST_DIR, { recursive: true });
  const templateSrc = await fsp.readFile(TEMPLATE_PATH, 'utf8');
  const css = await fsp.readFile(CSS_PATH, 'utf8');

  const template = Handlebars.compile(templateSrc);
  const files = await collectFiles();

  let highlighter = null;
  if (createHighlighter) {
    try {
      highlighter = await createHighlighter({
        themes: ['github-dark'],   // cargas el/los temas que vas a usar
        langs: ['cpp', 'text']
      });
      console.log('✅ Highlighter inicializado');
    } catch (error) {
      console.log('⚠️ Error al inicializar highlighter:', error.message);
    }
  }

  const entries = [];
  for (const file of files) {
    const raw = await fsp.readFile(file, 'utf8');
    const { data, body } = extractFrontmatter(raw, file);
    const lang = pickLanguageByExt(file);
    
    let codeHtml;
    if (highlighter) {
      codeHtml = highlighter.codeToHtml(body, { lang, theme: 'github-light' });
    } else {
      // Fallback sin syntax highlighting
      codeHtml = `<pre><code>${body.replace(/</g, '&lt;').replace(/>/g, '&gt;')}</code></pre>`;
    }
    
    console.log(data);
    entries.push({
      id: file.replace(/[\/\\]/g, '_'),
      path: file,
      title: data.title || path.basename(file),
      category: data.category || null,
      author: data.author || null,
      codeHtml
    });
  }

  // opcional: agrupa por categoría
  const byCategory = {};
  for (const e of entries) {
    const key = e.category || 'Sin categoría';
    if (!byCategory[key]) byCategory[key] = [];
    byCategory[key].push(e);
  }

  const html = template({
    generatedAt: new Date().toISOString(),
    css,
    categories: Object.keys(byCategory).sort().map(k => ({
      name: k,
      files: byCategory[k]
    }))
  });

  const htmlPath = path.join(DIST_DIR, 'notebook.html');
  await fsp.writeFile(htmlPath, html, 'utf8');
  return htmlPath;
}

async function htmlToPdf(htmlPath) {
  if (!playwrightAvailable) {
    console.log('⚠️ Playwright no disponible, saltando generación de PDF');
    return null;
  }

  try {
    const pdfPath = path.join(DIST_DIR, 'notebook.pdf');
    const browser = await chromium.launch();
    const page = await browser.newPage();

    // Carga el HTML por file:// para incluir estilos embebidos
    await page.goto('file://' + htmlPath.replace(/\\/g, '/'), { waitUntil: 'load' });

    await page.pdf({
      path: pdfPath,
      format: 'A4',
      printBackground: true,
      margin: { top: '20mm', right: '15mm', bottom: '20mm', left: '15mm' },
      displayHeaderFooter: true,
      headerTemplate: `
        <div style="font-size:10px; width:100%; text-align:right; padding-right:10mm;">
          Algonotes — {{date}}
        </div>`.replace('{{date}}', new Date().toLocaleDateString()),
      footerTemplate: `
        <div style="font-size:10px; width:100%; text-align:center;">
          <span class="pageNumber"></span> / <span class="totalPages"></span>
        </div>`
    });

    await browser.close();
    return pdfPath;
  } catch (error) {
    console.log('❌ Error generando PDF:', error.message);
    return null;
  }
}

(async () => {
  const htmlPath = await buildHtml();
  console.log('HTML:', htmlPath);
  
  const pdfPath = await htmlToPdf(htmlPath);
  if (pdfPath) {
    console.log('PDF :', pdfPath);
  } else {
    console.log('PDF : No generado (Playwright no disponible)');
  }
})();
