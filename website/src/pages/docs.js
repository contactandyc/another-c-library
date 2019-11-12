import React from "react"
import { Link, graphql } from "gatsby"

import Layout from "../components/layout"
import SEO from "../components/seo"
import Sidebar from "../components/sidebar"

function Docs({ data }) {
  const allPages = data.allMarkdownRemark
  const styles = {
    main: {
      margin: `0 auto`,
      maxWidth: `1200px`,
      display: `flex`
    },
    content: {
      color: `black`,
      margin: `0`,
      padding: `20px`
    },
    sidebar: {
      background: `#F7FAFC`,
      height: `100vh`,
      padding: `15px`,
      color: `#63B3ED`,
      position: `sticky`,
      top: `62px`,
    },
    code: {
      background: `#f6f8fa`,
      padding: `16px`,
      fontFamily: `SFMono-Regular,Consolas,Liberation Mono,Menlo,monospace`,
      lineHeight: `32px`,
      overflow: `scroll`
    }
  }

  return (
    <Layout>
      <SEO title="Docs" />
      <div style={styles.main}>
        <div style={styles.content} className="Content">
          <h1 size="2xl" m="20px auto">Official Documentation</h1>
          <p>To get started, clone the library:</p>
          <pre style={styles.code}>git clone https://github.com/contactandyc/another-c-library.git<br />
          cd another-c-library/demo<br />
          make<br />
          </pre>
          <p>The package depends on libuv in the uvdemo directory. On a mac, use the following command to install libuv:</p>
          <pre style={styles.code}>brew install libuv<br />
          cd another-c-library/uvdemo<br />
          make<br />
          </pre>
          <p>Usage and documentation are currently being written. Check out our eBook
          for a walk through of the library or help us write the docs by <Link to="/contributing">contributing!</Link></p>
          <p>Our goal is to produce a work that is unencumbered by licenses,
          and hope that others will find this code or parts of it useful. The library,
          book, code, website, and project in its entirety are licensed under the Apache License by Andy Curtis & Daniel Curtis. <Link to="/license">Read more on the licensing.</Link></p>

        </div>
        {/*<Sidebar allPages={allPages} type="docs" />*/}
      </div>
    </Layout>
  );
}

export default Docs

export const pageQuery = graphql`
  query DocsQuery {
    allMarkdownRemark(
      sort: { order: ASC, fields: [frontmatter___title] }
      filter: { frontmatter: { posttype: { eq: "doc" }}}
    ) {
      edges {
        node {
          id
          frontmatter {
            title
            path
          }
          headings(depth: h1) {
            value
          }
        }
      }
    }
  }
`
