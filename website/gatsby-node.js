const path = require(`path`)
const { createFilePath } = require(`gatsby-source-filesystem`)

exports.createPages = async ({ graphql, actions }) => {
  const { createPage } = actions

  //const docsPage = path.resolve(`./src/templates/docs-page.js`)
  const ebookPage = path.resolve(`./src/templates/ebook-page.js`)
  const result = await graphql(
    `
      {
        allMarkdownRemark(
          sort: { fields: [frontmatter___title], order: DESC }
          limit: 1000
        ) {
          edges {
            node {
              fields {
                slug
              }
              frontmatter {
                title
                posttype
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
    const previous = index === posts.length - 1 ? null : posts[index + 1].node
    const next = index === 0 ? null : posts[index - 1].node

    console.log("frontmatter  ", posts.frontmatter)

    if (post.node.frontmatter.posttype === "ebook") {
      createPage({
        path: post.node.fields.slug,
        component: ebookPage,
        context: {
          slug: post.node.fields.slug,
          previous,
          next,
        },
      })
    }
    /*else if (post.node.frontmatter.posttype === "doc") {
      createPage({
        path: post.node.fields.slug,
        component: docsPage,
        context: {
          slug: post.node.fields.slug,
          previous,
          next,
        },
      })
    }*/
  })
}

exports.onCreateNode = ({ node, actions, getNode }) => {
  const { createNodeField } = actions

  if (node.internal.type === `MarkdownRemark`) {
    let value = createFilePath({ node, getNode })

    /*if (node.frontmatter.posttype === "doc")
      value = '/docs' + value*/
    if (node.frontmatter.posttype === "ebook")
      value = '/ebook' + value

    createNodeField({
      name: `slug`,
      node,
      value,
    })
  }
}
