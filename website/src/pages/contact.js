import React from "react";
import { FaExternalLinkAlt } from "react-icons/fa";

import Layout from "../components/layout";
import SEO from "../components/seo";
import "./index.css"; // uses license.js styles

function Contact() {
  return (
    <Layout>
      <SEO title="Contact" />
      <div className="License-mn">
        <h1>Contact</h1>
        <p>Currently, the creators of this library are open to new professional
        opportunities as well as suggestions reguarding the library.</p>
        <h2 style={{marginBottom: `8px`}}>Andy Curtis</h2>
        <p><strong>Senior Software Engineer & Chief Architect</strong><br />
        <a href="https://linkedin.com/in/andycurtis" target="_blank" rel="noopener noreferrer">LinkedIn <FaExternalLinkAlt /></a><br />
        <a href="mailto:contactandyc@gmail.com">contactandyc@gmail.com <FaExternalLinkAlt /></a>
        </p>
        <h2 style={{marginBottom: `8px`}}>Daniel Curtis</h2>
        <p><strong>Software Engineer</strong><br />
        <a href="https://linkedin.com/in/dancurtis" target="_blank" rel="noopener noreferrer">LinkedIn <FaExternalLinkAlt /></a><br />
        <a href="mailto:contactdcurtis@gmail.com">contactdcurtis@gmail.com <FaExternalLinkAlt /></a>
        </p>
      </div>
    </Layout>
  );
}

export default Contact;
