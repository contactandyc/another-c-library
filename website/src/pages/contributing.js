import React from "react";
import { FaExternalLinkAlt } from "react-icons/fa";
import { Link } from "gatsby";

import Layout from "../components/layout";
import SEO from "../components/seo";
import "./index.css"; // uses license.js styles

function Contributing() {
  return (
    <Layout>
      <SEO title="Contributing" />
      <div className="License-mn">
        <h1>Contributing</h1>
        <p>Another C Library is under very active development. Our hope is that others
        will contribute! While we are in the early stages of library creation, please
        feel free to <Link to="/contact">contact</Link> the project maintainers.</p>
        <h2>Code of Conduct</h2>
        <p>We expect contributors to adhere to Contributor
        Covenant <a href="https://www.contributor-covenant.org/version/1/4/code-of-conduct" target="_blank" rel="noopener noreferrer">Code of Conduct <FaExternalLinkAlt />.</a> Please
        read over it to understand the standards and responsibilities of contributing to Open Source.</p>
        <h2>License</h2>
        <p>By contributing to Another C Library, you agree that your contributions will be licensed under its Apache License.</p>
      </div>
    </Layout>
  );
}

export default Contributing;
