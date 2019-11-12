const { hot } = require("react-hot-loader/root")

// prefer default export if available
const preferDefault = m => m && m.default || m


exports.components = {
  "component---src-templates-ebook-page-js": hot(preferDefault(require("/Users/dcurtis/projects/another-c-library/website/src/templates/ebook-page.js"))),
  "component---cache-dev-404-page-js": hot(preferDefault(require("/Users/dcurtis/projects/another-c-library/website/.cache/dev-404-page.js"))),
  "component---src-pages-404-js": hot(preferDefault(require("/Users/dcurtis/projects/another-c-library/website/src/pages/404.js"))),
  "component---src-pages-contact-js": hot(preferDefault(require("/Users/dcurtis/projects/another-c-library/website/src/pages/contact.js"))),
  "component---src-pages-contributing-js": hot(preferDefault(require("/Users/dcurtis/projects/another-c-library/website/src/pages/contributing.js"))),
  "component---src-pages-docs-js": hot(preferDefault(require("/Users/dcurtis/projects/another-c-library/website/src/pages/docs.js"))),
  "component---src-pages-ebook-js": hot(preferDefault(require("/Users/dcurtis/projects/another-c-library/website/src/pages/ebook.js"))),
  "component---src-pages-index-js": hot(preferDefault(require("/Users/dcurtis/projects/another-c-library/website/src/pages/index.js"))),
  "component---src-pages-license-js": hot(preferDefault(require("/Users/dcurtis/projects/another-c-library/website/src/pages/license.js")))
}

