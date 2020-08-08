# Gatsby MDX Netlify CMS Documentation Starter

An accessible and _blazing_ fast documentation starter for Gatsby integrated with Netlify CMS. Based on [Gatsby MDX Netlify CMS Starter](https://github.com/renvrant/gatsby-mdx-netlify-cms-starter).

Spend more time building and less time on docs! [Live Demo](https://mdx-cms-docs.netlify.app)

[![Netlify Status](https://api.netlify.com/api/v1/badges/dc60d64b-ffee-48d4-ba2d-5be6cb31e470/deploy-status)](https://app.netlify.com/sites/mdx-cms-docs/deploys)

[![Deploy to Netlify](https://www.netlify.com/img/deploy/button.svg)](https://app.netlify.com/start/deploy?repository=https://github.com/danielcurtis/gatsby-starter-netlify-docs)

## Features

- 📖 &nbsp; Netlify CMS for Managing Content
- 😎 &nbsp; All of the **Awesome** Benefits of Gatsby MDX
- ♿ &nbsp; Accessible and Fast. Scores 100's in every Lighthouse Category
- 🖥️ &nbsp; Landing Page Fully-Configurable from Netlify CMS
- 🧭 &nbsp; Table of Contents Sub-Menu & Toggleable Main-Menu
- ⏭️ &nbsp; Configurable Next & Previous Buttons
- 🧛‍♀️ &nbsp; Dark Mode
- 📱 &nbsp; 100% Repsponsive
- 📊 &nbsp; Google Anayltics
- 👩‍💻 &nbsp; Functional Components & React Hooks
- 🔷 &nbsp; Unopinionated, Minimal CSS

## Get started

Ensure you have NodeJS and the Gatsby CLI installed. If you don't read [these instructions](https://www.gatsbyjs.org/tutorial/part-zero/).

**Create the project**

```
gatsby new gatsby-starter-netlify-docs https://github.com/danielcurtis/gatsby-starter-netlify-docs
```

**Start the project**

```
cd gatsby-starter-netlify-docs
npm install
gatsby develop
```

_You may see some security warnings on this step. Those are being worked on._

The starter will start up at [localhost:8000](http://localhost:8000).

**Netlify CMS**

1. After cloning the project, create a repo on GitHub. Don't push your project yet!
2. Change the repo listed in **static/admin/config.yml** to your newly created repo.
3. Commit and push your changes. _Note, the default branch is set to main in the config._

After committing, you can start the project up again with `gatsby develop` and you should get an output from the CLI for the URL to visit the CMS UI.

You should be able to access the CMS locally and make changes. Changes are pushed directly to main, so you'll have to pull them from origin to see them.

In production, you can make changes at https://your-awesome-site/admin. Netlify CMS uses GitHub for authentication. You'll have to get API keys from GitHub, [as described in Netlify's docs.](https://docs.netlify.com/visitor-access/oauth-provider-tokens/#using-an-authentication-provider)

## Contributing

I'm definitely open to contributions! I'd love to see this project grow beyond me. However, not every PR will be merged. The general guidelines for contributing are:

- Does it make development easier / better?
- Does it improve cross-platforming (browsers/mobile)?
- Does it fix a bug?
- Does it break anything? _Testing 1..2.... :(_
- Does it reduce the build size?
- Does it add a feature without adding complexity?

Please, create an issue before spending too much time on a PR so we can discuss the changes!

## Roadmap

- [x] Add syntax highlighting _v1.0.1_
- [ ] Resolve Security Warnings
- [ ] Add tests
