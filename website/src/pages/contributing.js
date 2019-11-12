import React from "react"
import { FaExternalLinkAlt } from "react-icons/fa"
import { Link } from "gatsby"

import Layout from "../components/layout"
import SEO from "../components/seo"

function Contributing() {
  const styles = {
    main: {
      margin: `0 auto`,
      maxWidth: `1200px`,
      minHeight: `60vh`,
      padding: `50px 10px`
    }
  }

  return (
    <Layout>
      <SEO title="License" />
      <div style={styles.main}>
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

export default Contributing
