import React, { useState, useEffect } from "react";
import { Helmet } from "react-helmet";
import { Link, graphql } from "gatsby";
import { FaArrowAltCircleLeft, FaArrowAltCircleRight } from "react-icons/fa";

import Layout from "../components/layout";
import SEO from "../components/seo";
import MdContent from "../components/mdcontent";
import Sidebar from "../components/sidebar";
import "../pages/index.css";

function MdPages({ data }) {
  const currPage = data.markdownRemark;
  const currTitle = data.markdownRemark.frontmatter.title;
  const allPages = data.allMarkdownRemark;
  const currPos = parseInt(currTitle);

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

  return (
    <Layout>
      <SEO title={currTitle} />
      <Helmet title={currTitle} />
      <div className="Md-mn">
        <div className="Md-cntnt Content">
          <h1>{currTitle}</h1>
          <p>Copyright {new Date().getFullYear()} Andy Curtis & Daniel Curtis</p>
          <MdContent content={currPage.html} />
          <div className="Md-pst-ftr">
            <Link to={`/${currPage.frontmatter.posttype}/${prev.node.frontmatter.path}`}>
              <FaArrowAltCircleLeft className="Md-rrw" /> {prev.node.frontmatter.title}
            </Link>
            <Link to={`/${currPage.frontmatter.posttype}/${next.node.frontmatter.path}`}>
              {next.node.frontmatter.title} <FaArrowAltCircleRight className="Md-rrw" />
            </Link>
          </div>
        </div>

        <Sidebar
          current={currTitle}
          allPages={allPages}
          type={currPage.frontmatter.posttype} />

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
`
