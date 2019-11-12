import React from "react"
import { FaExternalLinkAlt } from "react-icons/fa"

import Layout from "../components/layout"
import SEO from "../components/seo"

function Contact() {
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
        <h1>Contact</h1>
        <p>Currently, the creators of this library are open to new opportunities.</p>
        <h2 style={{marginBottom: `8px`}}>Andy Curtis</h2>
        <p><strong>Senior Software Engineer & Chief Architect</strong><br />
        <a href="https://andycurtis.me" target="_blank" rel="noopener noreferrer">https://AndyCurtis.me <FaExternalLinkAlt /></a><br />
        <a href="mailto:contactandyc@gmail.com">contactandyc@gmail.com <FaExternalLinkAlt /></a>
        </p>
        <h2 style={{marginBottom: `8px`}}>Daniel Curtis</h2>
        <p><strong>Full-stack developer</strong><br />
        <a href="https://linkedin/in/dancurtis" target="_blank" rel="noopener noreferrer">LinkedIn <FaExternalLinkAlt /></a><br />
        <a href="mailto:daniel17curtis@gmail.com">daniel17curtis@gmail.com <FaExternalLinkAlt /></a>
        </p>
      </div>
    </Layout>
  );
}

export default Contact
