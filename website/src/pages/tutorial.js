import React from "react";
import { Link, graphql } from "gatsby";
import { FaExternalLinkAlt } from "react-icons/fa";

import Layout from "../components/layout";
import SEO from "../components/seo";
import Sidebar from "../components/sidebar";
import "./index.css";

function Tutorial({ data }) {
  const allPages = data.allMarkdownRemark;

  return (
    <Layout>
      <SEO title="Learn C Programming" />

      <div className="Tutorial-mn">
        <div className="Tutorial-cntnt Content">
          <h1>Learn C Programming</h1>
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

          <p><strong>For a more in depth usage guide without a tutorial check out
          <Link to="/docs/"> our docs.</Link></strong></p>
        </div>

        <Sidebar allPages={allPages} type="tutorial" />
      </div>
    </Layout>
  );
}

export default Tutorial;

export const pageQuery = graphql`
  query TutorialQuery {
    allMarkdownRemark(
      sort: { order: ASC, fields: [frontmatter___title] }
      filter: { frontmatter: { posttype: { eq: "tutorial" }}}
    ) {
      edges {
        node {
          id
          frontmatter {
            title
            path
            posttype
          }
          headings(depth: h2) {
            value
          }
        }
      }
    }
  }
`
