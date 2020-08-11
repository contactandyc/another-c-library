# Another C Library Documentation

## File Structure

- **Docs** are located in **website/src/pages/docs**
- **Tutorial pages** are located in **website/src/pages/tutorial**

Page URLs will be generated from the given filename and located at */docs/your-filename* or */tutorial/your-filename*. The title and description for the page must be added at the top of the markdown file:

```md
---
title: Your Super Cool Page
description: A super cool page about nothin.
---
```

You can embed almost anything from YouTube to Repl.it. Learn more on the [Gatsby docs starter's page](https://mdx-cms-docs.netlify.app/tutorial/1-introduction-mdx).

If you create a new page, be sure to add it to the naviagtion in the **website/src/pages/config.md** file. 

To add next and previous buttons at the bottom of the page, add the following component:

```js
<NextPrev prev="Prev page title" prevUrl="/docs/prev-page-url" next="Next title" nextUrl="/docs/next-page-url" />
```

## Contributing

### Guidelines

We want contributing to Another C Library to be both enjoyable and educational for anyone and everyone. All contributions are welcome, including issues, docs, tutorials, examples, tests, and etc. Since you're looking at the docs website directory, there's a few guidelines we follow before merging PRs:

- Does it make the docs/tutorial clearer / better?
- Does it fix grammatical mistakes?
- Does it fix a bug with the website?
- Does it break anything?

If you're implementing major changes, please create an issue before spending too much time so we can discuss the changes.

### Contributing via Local Development

**Start the local dev server:**

1. Fork the repo
2. Switch to the website directory
3. Ensure you have NodeJS and the [gatsby cli](https://www.gatsbyjs.org/tutorial/part-zero/) installed
4. Install dependancies `npm i`
5. Start the local dev server `gatsby develop`

**Make Changes:**

Create or update documentation, tutorials, or pages in the website/src/pages/ directory. Be sure to manually test your changes!

**Open a PR:**

1. Create a PR from your fork
2. Please, label the PR **(docs/tutorial update): describe your changes**

### Contributing via Netlify CMS

If you're a core maintainer, you have access to edit documentation and tutorials via Netlify CMS at [anotherclibrary.com/admin](https://anotherclibrary.com/admin).

**Note, saving files here will make direct pushes to master.**
