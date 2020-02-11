import React from "react";
import { Helmet } from "react-helmet";
import { Link, graphql } from "gatsby";

import Layout from "../components/layout";
import SEO from "../components/seo";
import MdContent from "../components/mdcontent";
import "../pages/index.css";

function MdPages(props) {
  const { data } = props;
  const currPage = data.markdownRemark;
  const currTitle = data.markdownRemark.frontmatter.title;
  const { tutArr, prev, next } = props.pageContext;

  console.log(tutArr);

  return (
    <Layout type={data.markdownRemark.frontmatter.posttype}>
      <SEO title={currTitle} />
      <Helmet title={currTitle} />
      <div className="Content">
        <h1>{currTitle}</h1>
        <p>Copyright {new Date().getFullYear()} Andy Curtis & Daniel Curtis</p>
        <MdContent content={currPage.html} />
      </div>
      <ul
        style={{
          display: `flex`,
          flexWrap: `wrap`,
          justifyContent: `space-between`,
          listStyle: `none`,
          padding: 0,
        }}
      >
        <li>
          {prev && (
            <Link to={prev.fields.slug} rel="prev">
              ← {prev.frontmatter.title}
            </Link>
          )}
        </li>
        <li>
          {next && (
            <Link to={next.fields.slug} rel="next">
              {next.frontmatter.title} →
            </Link>
          )}
        </li>
      </ul>
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
