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
        feel free to <Link to="/contact">contact</Link> the project creators
        or open a pull request!</p>

        <h2>Project setup</h2>
        <ol>
          <li>Fork and clone the repo</li>
          <li>Ensure libuv is installed</li>
          <li>Create a branch for your PR with git checkout -b pr/your-branch-name</li>
        </ol>

        <p>Keep your master branch pointing at the original repository and make pull requests from branches on your fork. To do this, run:</p>
        <pre style={{ padding: `16px` }}>git remote add upstream https://github.com/andycurtis/another-c-library.git<br />
          git fetch upstream<br />
          git branch --set-upstream-to=upstream/master master<br />
        </pre>

        <h2>Committing and Pushing changes</h2>
        <p>Please make sure to run tests before you commit your changes.</p>
        <p>All changes will be reviewed by the project maintainers.</p>
        <p><strong>By contributing to Another C Library, you agree that your contributions will be licensed under its Apache License.</strong></p>
      </div>
    </Layout>
  );
}

export default Contributing;
