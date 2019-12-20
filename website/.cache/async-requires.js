// prefer default export if available
const preferDefault = m => m && m.default || m

exports.components = {
  "component---src-templates-ebook-page-js": () => import("./../src/templates/ebook-page.js" /* webpackChunkName: "component---src-templates-ebook-page-js" */),
  "component---cache-dev-404-page-js": () => import("./dev-404-page.js" /* webpackChunkName: "component---cache-dev-404-page-js" */),
  "component---src-pages-404-js": () => import("./../src/pages/404.js" /* webpackChunkName: "component---src-pages-404-js" */),
  "component---src-pages-contact-js": () => import("./../src/pages/contact.js" /* webpackChunkName: "component---src-pages-contact-js" */),
  "component---src-pages-contributing-js": () => import("./../src/pages/contributing.js" /* webpackChunkName: "component---src-pages-contributing-js" */),
  "component---src-pages-docs-js": () => import("./../src/pages/docs.js" /* webpackChunkName: "component---src-pages-docs-js" */),
  "component---src-pages-ebook-js": () => import("./../src/pages/ebook.js" /* webpackChunkName: "component---src-pages-ebook-js" */),
  "component---src-pages-index-js": () => import("./../src/pages/index.js" /* webpackChunkName: "component---src-pages-index-js" */),
  "component---src-pages-license-js": () => import("./../src/pages/license.js" /* webpackChunkName: "component---src-pages-license-js" */)
}

