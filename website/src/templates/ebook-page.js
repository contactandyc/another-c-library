import React, { useState, useEffect } from "react"
import { Helmet } from "react-helmet"
import { Link, graphql } from "gatsby"
import { FaArrowAltCircleLeft, FaArrowAltCircleRight } from "react-icons/fa"

import Layout from "../components/layout"
import SEO from "../components/seo"
import MdContent from "../components/mdcontent"
import Sidebar from "../components/sidebar"

function EbookPage({ data }) {
  const currentPage = data.markdownRemark
  const allPages = data.allMarkdownRemark
  const currPos = parseInt(currentPage.frontmatter.title);

  allPages.edges.sort(function(a, b) {
    let ai = parseInt(a.node.frontmatter.title);
    let bi = parseInt(b.node.frontmatter.title);
    return ai - bi;
  });

  let prev = { node: { frontmatter: { title: "Home", path: "/" }}};
  let next = { node: { frontmatter: { title: "Home", path: "/" }}};

  if (currPos + 2 < allPages.edges.length && currPos > 1) {
    next = allPages.edges[currPos + 2];
    prev = allPages.edges[currPos];
  } else if (currPos <= 1) {
    next = allPages.edges[currPos + 2];
  } else if (currPos + 2 >= allPages.edges.length) {
    prev = allPages.edges[currPos];
  }


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
      width: `500px`,
      height: `100vh`,
      padding: `15px`,
      color: `#63B3ED`,
      position: `sticky`,
      top: `62px`,
    },
    activeLink: {
      color: `#38A169`,
      fontWeight: `700`
    },
    postFooter: {
      display: `flex`,
      justifyContent: `space-between`,
      borderTop: `solid 1px black`,
      paddingTop: `10px`
    },
    arrow: {
      paddingTop: `5px`
    }
  }


  return (
    <Layout>
      <SEO title={currentPage.frontmatter.title} />
      <Helmet title={`${currentPage.frontmatter.title}`} />
      <div style={styles.main}>

        <div style={styles.content} className="Content">
          <h1>{currentPage.frontmatter.title}</h1>
          <p>Copyright {new Date().getFullYear()} Andy Curtis & Daniel Curtis</p>
          <MdContent content={currentPage.html} />
          <div style={styles.postFooter}>
            <Link to={`/ebook/${prev.node.frontmatter.path}`}>
              <FaArrowAltCircleLeft style={styles.arrow}/> {prev.node.frontmatter.title}
            </Link>
            <Link to={`/ebook/${next.node.frontmatter.path}`}>
              {next.node.frontmatter.title} <FaArrowAltCircleRight style={styles.arrow} />
            </Link>
          </div>
        </div>

        <Sidebar
          allPages={allPages}
          type="ebook" />

      </div>
    </Layout>
  )
}

export default EbookPage
// filter: { frontmatter: { posttype: { eq: "ebook" }}} below sort in allMarkdownRemark
export const pageQuery = graphql`
  query EbookPostBySlug($slug: String!) {
    site {
      siteMetadata {
        title
      }
    }
    markdownRemark(fields: { slug: { eq: $slug } }) {
      id
      html
      frontmatter {
        title
      }
    }
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
