import React from "react";
import PropTypes from "prop-types";
import { useStaticQuery, graphql } from "gatsby";

import Header from "./header";
import Footer from "./footer";
import Sidebar from "./sidebar";

function Layout({ type, children }) {
  const data = useStaticQuery(graphql`
    query SiteTitleQuery {
      site {
        siteMetadata {
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
              posttype
            }
            headings(depth: h2) {
              value
            }
          }
        }
      }
    }
  `);

  if (type === "tutorial" || type === "docs") {
    return (
      <div>
        <Header siteTitle={data.site.siteMetadata.title} />
        <div className="Layout">
          <main className="Content">{children}</main>
          <Sidebar pages={data.allMarkdownRemark} type={type} />
        </div>
        <Footer />
      </div>
    );
  }
  else {
    return (
      <div>
        <Header siteTitle={data.site.siteMetadata.title} />
        <main>{children}</main>
        <Footer />
      </div>
    );
  }
}

Layout.propTypes = { children: PropTypes.node.isRequired };

export default Layout;
