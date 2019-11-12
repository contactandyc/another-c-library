import React from "react"
import { Link, graphql } from "gatsby"
import { FaExternalLinkAlt } from "react-icons/fa"

import Layout from "../components/layout"
import SEO from "../components/seo"
import Sidebar from "../components/sidebar"

export default function Ebook({ data }) {
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
      width: `20vw`,
      height: `100vh`,
      padding: `15px`,
      color: `#63B3ED`,
      position: `sticky`,
      top: `62px`,
    }
  }

  return (
    <Layout>
      <SEO title="A C Programming Language eBook" />
      <div style={styles.main}>
        <div style={styles.content} className="Content">
          <h1>A C Programming Language eBook</h1>
          <p>The book/library started in August 2019, so it's a work in progress.
          This is not a usage book. We're working on a book that simply explains usage.
          We follow patterns used by <a href="https://nikhilm.github.io/uvbook/index.html" target="_blank" rel="noopener noreferrer">https://nikhilm.github.io/uvbook/index.html <FaExternalLinkAlt /></a>.
          You create velocity by slowing down and making sure that you always
          have understanding of what you are doing (or at a minimum make sure you have
            an understanding of what you don't understand). Developers get better when
            they can explain what they know (and this is independent of language or
              technology). Becoming a great developer takes practice. This book may
              take several reads before you fully get it. The examples intentionally
              build upon each other, but may build too quickly for some. Feel free to <Link to="/contact">contact us</Link> with questions.</p>
        </div>

        <Sidebar allPages={allPages} type="ebook" />

      </div>
    </Layout>
  );
}
// filter: { frontmatter: { posttype: { eq: "ebook" }}} below sort in allMarkdownRemark

export const pageQuery = graphql`
  query EbookQuery {
    allMarkdownRemark(
      sort: { order: ASC, fields: [frontmatter___title] }
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
