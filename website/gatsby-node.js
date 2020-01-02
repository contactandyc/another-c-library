const path = require(`path`)
const { createFilePath } = require(`gatsby-source-filesystem`)

exports.createPages = async ({ graphql, actions }) => {
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
          }
        }
      }
    `
  )

  if (result.errors) {
    throw result.errors
  }

  // Create blog posts pages.
  const posts = result.data.allMarkdownRemark.edges

  posts.forEach((post, index) => {
    // const previous = index === posts.length - 1 ? null : posts[index + 1].node
    // const next = index === 0 ? null : posts[index - 1].node

    if (post.node.frontmatter.posttype === "ebook") {
      createPage({
        path: post.node.fields.slug,
        component: ebookPage,
        context: {
          slug: post.node.fields.slug,
          // previous,
          // next,
        },
      })
    }
    else if (post.node.frontmatter.posttype === "usage") {
      createPage({
        path: post.node.fields.slug,
        component: usagePage,
        context: {
          slug: post.node.fields.slug,
          //previous,
          //next,
        },
      })
    }
  })
}

exports.onCreateNode = ({ node, actions, getNode }) => {
  const { createNodeField } = actions

  if (node.internal.type === `MarkdownRemark`) {
    let value = createFilePath({ node, getNode })

    if (node.frontmatter.posttype === "usage")
      value = '/usage' + value
    if (node.frontmatter.posttype === "ebook")
      value = '/ebook' + value

    createNodeField({
      name: `slug`,
      node: node,
      value: value
    })
  }
}
