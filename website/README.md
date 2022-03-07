# Another C Library Website

The code behind [AnotherCLibrary.com](https://AnotherCLibrary.com).
Currently, the website is built using React and GatsbyJS and renders the
markdown files in the Another C Library GitHub repo.

## Quick start

Ensure you're in the root website directory and have the latest code `git pull`

1. Ensure you're using node version 14: `node --version`
2. If you're not, use `nvm use v14` to switch (https://github.com/nvm-sh/nvm)
3. Run `npm install`
4. Run `npm run develop` to run your project locally at http:localhost:8000
5. Run `npm run build` before committing code to ensure it builds

## Adding plain HTML/JS/CSS pages

Everything inside the /static folder will be served, but it must be static (can't fetch data). To add a new static file without React, navigate to the static folder and add a new directory for your files.

For example, an index.html file inside the /static/games/minesweeper directory will be available at localhost:8000/games/minesweeper.

## Adding react pages

To add a new react page, go to /src/pages and add a new page. If your page has a lot of repeating code, split it out into a component in the /src/components directory.

## Architecture

The site currently queries markdown files from the Another C Library repo in the
docs folder. These files are encapsulated within another file which gives them
their URL alias. At the top of every file, frontmatter (metadata) is added including
the page type, name, and path (for internal linking only).

The bulk of the code is located in the /src folder. Here, you will find:

- components (for page components such as the header)
- images (for lazy-loaded images, currently none except favicon)
- pages (for the pages not rendered from markdown files)
- templates (templates for pages that are rendered from markdown files)
- utils (additional utility functions, such as typography)

The src folder renders the public folder at build.

The documentation site is hosted on [Netlify](https://netlify.com), which produces
the latest build on every commit to master on the Another C Library repo.

## Licensing

Throughout the eBook and documentation on this website, the following notice will
appear at the top of most of the files:

Copyright 2019 Andy Curtis & Daniel Curtis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

Our goal is to produce a work that is unencumbered by licenses, and hope that
others will find this code or parts of it useful.

The website itself was built using React and GatsbyJS which are both licensed
under the MIT License.
