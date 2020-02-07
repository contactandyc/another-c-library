import React from "react";
import { Link } from "gatsby";

import Layout from "../components/layout";
import SEO from "../components/seo";

function Tutorial() {
  return (
    <Layout type="tutorial">
      <SEO title="Tutorial" />
      <div>
        <h1>Tutorial</h1>
        <p>
          The book/library started in August 2019, so it's a work in progress.
          This is not a usage book. We're working on a book that simply
          explains usage. You create velocity by slowing down and making sure
          that you always have understanding of what you are doing (or at a
          minimum make sure you have an understanding of what you don't
          understand). Developers get better when they can explain what they
          know (and this is independent of language or technology). Becoming a
          great developer takes practice. The tutorial may take several reads
          before you fully get it. The examples intentionally build upon each
          other, but may build too quickly for some. Feel free to
          <Link to="/contact"> contact us </Link>with questions.
        </p>
        <p>
          <strong>For a more in depth usage guide without a tutorial check out
          <Link to="/docs/"> our docs.</Link></strong>
        </p>
      </div>
    </Layout>
  );
}

export default Tutorial;
