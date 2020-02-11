const path = require(`path`);
const { createFilePath } = require(`gatsby-source-filesystem`);

exports.createPages = async ({ graphql, actions }) => {
  const { createPage } = actions;
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
            posttype
          }
        }
      }
    }
  }`);

  if (result.errors)
    throw result.errors;

  let allArr = result.data.allMarkdownRemark.edges.filter(i => i.node.frontmatter.title.length > 0);
  let tutArr = allArr.filter(i => i.node.frontmatter.posttype === "tutorial");
  let docArr = allArr.filter(i => i.node.frontmatter.posttype === "docs");

  tutArr.sort(function(a, b) {
    let ai = parseInt(a.node.frontmatter.title);
    let bi = parseInt(b.node.frontmatter.title);
    return ai - bi;
  });

  tutArr.forEach((post, i) => {
    const next = i === tutArr.length - 1 ? null : tutArr[i + 1].node;
    const prev = i === 0 ? null : tutArr[i - 1].node;

    createPage({
      path: post.node.fields.slug,
      component: mdPage,
      context: {
        slug: post.node.fields.slug,
        prev,
        next,
        tutArr
      },
    });
  });

  docArr.forEach((post, i) => {
    const prev = i === docArr.length - 1 ? null : docArr[i + 1].node;
    const next = i === 0 ? null : docArr[i - 1].node;

    createPage({
      path: post.node.fields.slug,
      component: mdPage,
      context: {
        slug: post.node.fields.slug,
        prev,
        next,
      },
    });
  });
}

exports.onCreateNode = ({ node, actions, getNode }) => {
  const { createNodeField } = actions

  if (node.internal.type === `MarkdownRemark`) {
    let value = createFilePath({ node, getNode });

    if (node.frontmatter.posttype === "tutorial")
      value = '/tutorial' + value;
    else if (node.frontmatter.posttype === "docs")
      value = '/docs' + value;

    createNodeField({
      name: `slug`,
      node: node,
      value: value
    });
  }
}
