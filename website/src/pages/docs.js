import React from "react";
import { Link, graphql } from "gatsby";

import Layout from "../components/layout";
import SEO from "../components/seo";
import Sidebar from "../components/sidebar";
import "./index.css"; // uses Tutorial.js styling

function Docs({ data }) {
  const allPages = data.allMarkdownRemark;

  return (
    <Layout>
      <SEO title="Docs" />
      <div className="Tutorial-mn">
        <div className="Tutorial-cntnt Content">
          <h1 size="2xl" m="20px auto">Official Documentation</h1>
          <p>To get started, clone the library:</p>
          <pre style={{ padding: `16px` }}>git clone https://github.com/contactandyc/another-c-library.git<br />
          cd another-c-library/demo<br />
          make<br />
          </pre>
          <p>The package depends on libuv in the uvdemo directory. On a mac, use the following command to install libuv:</p>
          <pre style={{ padding: `16px` }}>brew install libuv<br />
          cd another-c-library/uvdemo<br />
          make<br />
          </pre>
          <p>Usage and documentation are currently being written. Check out our Tutorial
          for a walk through of the library or help us write the docs by <Link to="/contributing">contributing!</Link></p>
          <p>Our goal is to produce a work that is unencumbered by licenses,
          and hope that others will find this code or parts of it useful. The library,
          book, code, website, and project in its entirety are licensed under the Apache License by Andy Curtis & Daniel Curtis. <Link to="/license">Read more on the licensing.</Link></p>

          <p><strong>For a more in depth tutorial on getting started, check out
          <Link to="/tutorial/1-getting-started"> Getting Started.</Link></strong></p>

        </div>

        <Sidebar allPages={allPages} type="docs" />

      </div>
    </Layout>
  );
}

export default Docs;

export const pageQuery = graphql`
  query DocsQuery {
    allMarkdownRemark(
      sort: { order: ASC, fields: [frontmatter___title] }
      filter: { frontmatter: { posttype: { eq: "docs" }}}
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
