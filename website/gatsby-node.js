const path = require(`path`);
const { createFilePath } = require(`gatsby-source-filesystem`);

exports.createPages = async ({ graphql, actions }) => {
<<<<<<< HEAD
  const { createPage } = actions;
  // const docsPage = path.resolve(`./src/templates/docs-page.js`);
  const mdPage = path.resolve(`./src/templates/md-page.js`);
  const result = await graphql(`{
    allMarkdownRemark(
      sort: { fields: [frontmatter___title], order: DESC }
    ) {
      edges {
        node {
          fields {
            slug
          }
          frontmatter {
            title
=======
  const { createPage } = actions

  const usagePage = path.resolve(`./src/templates/usage-page.js`)
  const ebookPage = path.resolve(`./src/templates/ebook-page.js`)
  const result = await graphql(
    `
      {
        allMarkdownRemark(
          sort: { fields: [frontmatter___title], order: DESC }
        ) {
          edges {
            node {
              fields {
                slug
              }
              frontmatter {
                title
              }
            }
>>>>>>> ec095e27a97f9e4d9e7a1924bf81c7de7402ae28
          }
        }
      }
    }
  }`);

  if (result.errors)
    throw result.errors

  // Create blog posts pages.
  const posts = result.data.allMarkdownRemark.edges

  posts.forEach((post, index) => {
    //const previous = index === posts.length - 1 ? null : posts[index + 1].node
    //const next = index === 0 ? null : posts[index - 1].node

<<<<<<< HEAD
    //if (post.node.frontmatter.posttype === "ebook") { // posttype bug
=======
    if (post.node.frontmatter.posttype === "ebook") {
>>>>>>> ec095e27a97f9e4d9e7a1924bf81c7de7402ae28
      createPage({
        path: post.node.fields.slug,
        component: mdPage,
        context: {
          slug: post.node.fields.slug
        },
<<<<<<< HEAD
      });
    /*} else if (post.node.frontmatter.posttype === "docs") {
=======
      })
    }
    else if (post.node.frontmatter.posttype === "usage") {
>>>>>>> ec095e27a97f9e4d9e7a1924bf81c7de7402ae28
      createPage({
        path: post.node.fields.slug,
        component: usagePage,
        context: {
<<<<<<< HEAD
          slug: post.node.fields.slug
        },
      });
    }*/
  });
=======
          slug: post.node.fields.slug,
          //previous,
          //next,
        },
      })
    }
  })
>>>>>>> ec095e27a97f9e4d9e7a1924bf81c7de7402ae28
}

exports.onCreateNode = ({ node, actions, getNode }) => {
  const { createNodeField } = actions

  if (node.internal.type === `MarkdownRemark`) {
    let value = createFilePath({ node, getNode });

<<<<<<< HEAD
    if (node.frontmatter.posttype === "tutorial")
      value = '/tutorial' + value;
    else if (node.frontmatter.posttype === "docs")
      value = '/docs' + value;
=======
    if (node.frontmatter.posttype === "usage")
      value = '/usage' + value
    if (node.frontmatter.posttype === "ebook")
      value = '/ebook' + value
>>>>>>> ec095e27a97f9e4d9e7a1924bf81c7de7402ae28

    createNodeField({
      name: `slug`,
      node: node,
      value: value
    });
  }
}
