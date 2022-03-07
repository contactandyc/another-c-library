const path = require(`path`);
const { createFilePath } = require(`gatsby-source-filesystem`);

const express = require(`express`);

// Enable development support for serving HTML from `./static` folder
exports.onCreateDevServer = ({ app }) => {
  app.use(express.static(`static`));
};

exports.createPages = async ({ graphql, actions }) => {
  const { createPage } = actions;
  // const docsPage = path.resolve(`./src/templates/docs-page.js`);
  const mdPage = path.resolve(`./src/templates/md-page.js`);
  const result = await graphql(`
    {
      allMarkdownRemark(sort: { fields: [frontmatter___title], order: DESC }) {
        edges {
          node {
            fields {
              slug
            }
            frontmatter {
              title
            }
          }
        }
      }
    }
  `);

  if (result.errors) throw result.errors;

  // Create blog posts pages.
  const posts = result.data.allMarkdownRemark.edges;

  posts.forEach((post, index) => {
    //const previous = index === posts.length - 1 ? null : posts[index + 1].node
    //const next = index === 0 ? null : posts[index - 1].node

    //if (post.node.frontmatter.posttype === "ebook") { // posttype bug
    createPage({
      path: post.node.fields.slug,
      component: mdPage,
      context: {
        slug: post.node.fields.slug
      }
    });
    /*} else if (post.node.frontmatter.posttype === "docs") {
      createPage({
        path: post.node.fields.slug,
        component: docsPage,
        context: {
          slug: post.node.fields.slug
        },
      });
    }*/
  });
};

exports.onCreateNode = ({ node, actions, getNode }) => {
  const { createNodeField } = actions;

  if (node.internal.type === `MarkdownRemark`) {
    let value = createFilePath({ node, getNode });

    if (node.frontmatter.posttype === 'tutorial') value = '/tutorial' + value;
    else if (node.frontmatter.posttype === 'docs') value = '/docs' + value;

    createNodeField({
      name: `slug`,
      node: node,
      value: value
    });
  }
};
