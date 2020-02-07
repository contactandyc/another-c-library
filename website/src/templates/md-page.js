import React from "react";
import { Helmet } from "react-helmet";
import { graphql } from "gatsby";

import Layout from "../components/layout";
import SEO from "../components/seo";
import MdContent from "../components/mdcontent";
import "../pages/index.css";

function MdPages({ data }) {
  const currPage = data.markdownRemark;
  const currTitle = data.markdownRemark.frontmatter.title;

  return (
    <Layout type={data.markdownRemark.frontmatter.posttype}>
      <SEO title={currTitle} />
      <Helmet title={currTitle} />
      <div className="Content">
        <h1>{currTitle}</h1>
        <p>Copyright {new Date().getFullYear()} Andy Curtis & Daniel Curtis</p>
        <MdContent content={currPage.html} />
      </div>
    </Layout>
  );
}

export default MdPages;

export const pageQuery = graphql`
  query MdPostBySlug($slug: String!) {
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
        posttype
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
            posttype
          }
          headings(depth: h2) {
            value
          }
        }
      }
    }
  }
`;
