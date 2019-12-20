import React from "react"
import { Link } from "gatsby"
import { FaArrowRight, FaBook } from "react-icons/fa"

import Layout from "../components/layout"
import SEO from "../components/seo"
import "./index.css"

export default function IndexPage() {
  const styles = {
    hero: {
      backgroundColor: `#2D3748`,
      height: `50vh`,
      display: `flex`,
      alignItems: `center`,
      textAlign: `center`,
      margin: `0 auto`
    },
    heroText: {
      textAlign: `center`,
      margin: `0 auto`,
    },
    h2: {
      fontSize: `calc(16px + 2vw)`,
      color: `white`
    },
    h3: {
      fontSize: `calc(16px + 1vw)`,
      color: `#E2E8F0`
    },
    code: {
      fontSize: `calc(16px + 1.5vw)`,
      backgroundColor: `#4A5568`,
      padding: `5px`
    },
    button: {
      borderRadius: `10px`,
      border: `none`,
      padding: `10px 24px`,
      margin: `10px`,
      fontSize: `18px`,
      fontFamily: `-apple-system,'BlinkMacSystemFont','Segoe UI','Roboto','Oxygen','Ubuntu','Cantarell','Fira Sans','Droid Sans','Helvetica Neue',sans-serif`,
      background: `#3182CE`,
      color: `white`,
      cursor: `pointer`
    },
    main: {
      maxWidth: `1200px`,
      margin: `80px auto`,
      padding: `10px`
    },
    p: {
      color: `#1A202C`,
      listStyle: `none`
    }
  }

  return (
    <Layout>
      <SEO title="Home" />
      <div style={styles.hero}>
        <div style={styles.heroText}>
          <h2 style={styles.h2}>Another C Library</h2>
          <h3 style={styles.h3}><code style={styles.code}>ac_</code> library for building scalable, complex applications.</h3>
            <Link to="/docs/"><button style={styles.button}>Get Started <FaArrowRight style={{ paddingTop: `5px`}}/></button></Link>
            <Link to="/ebook/"><button style={styles.button}>A C eBook <FaBook style={{ paddingTop: `5px`}}/></button></Link>
        </div>
      </div>

      <div style={styles.main} className="Flex">
        <div>
          <h3>Goals of this Project:</h3>
          <ol>
            <li>To provide an open source collection of algorithms necessary to build complex applications</li>
            <li>To help engineers understand algorithms and C better, so that they can create their own</li>
            <li>To help people to learn what it takes to create something new</li>
            <li>Build scalable applications using technology like Kubernetes, nginx, and docker</li>
          </ol>
        </div>
        <div>
          <h3>About this Project:</h3>
          <p style={styles.p}>Another C Library rebuilds, and in some cases, improves
          upon, some of the most useful Computer Science algorithms from the ground up.
          The library is Open Source and was created in late 2019, so it is
          currently still under very active development.</p>
          <p style={styles.p}>Andy and Daniel Curtis created this library to serve
          as a starting point for C developers. Whether you're diving into C for
          the first time or building complex applications, Another C Library offers
          tools for both learning and building.</p>
        </div>
      </div>
    </Layout>
  );
}
