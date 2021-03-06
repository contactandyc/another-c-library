import React from "react";

import Layout from "../components/layout";
import SEO from "../components/seo";

function NotFoundPage() {
  return (
    <Layout>
      <SEO title="404: Not found" />
      <h1>404: NOT FOUND</h1>
      <p>You just hit a route that does not exist.</p>
    </Layout>
  );
}

export default NotFoundPage;
