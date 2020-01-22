# Another C Library Website

The code behind [AnotherCLibrary.com](https://AnotherCLibrary.com).
Currently, the website is built using React and GatsbyJS and renders the
markdown files in the Another C Library GitHub repo.

## Get Started

Ensure you're in the root website directory.
Install the necessary packages using npm (may require sudo):

```bash
npm install
```

For local development run gatsby develop:

```bash
gatsby develop
```

The website will be running locally on port 8000 (localhost:8000)
For a build production, run gatsby build:

```bash
gatsby build
```

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
