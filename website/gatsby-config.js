module.exports = {
  siteMetadata: {
    title: `Another C Library`,
    description: `Another C Library rebuilds, and in some cases, improves upon, some of the most useful Computer Science algorithms from the ground up for engineers to build complex, scalable applications.`,
    author: `Daniel Curtis`,
    siteUrl: `https://www.anotherclibrary.com`,
  },
  plugins: [
    `gatsby-plugin-catch-links`,
    `gatsby-plugin-react-helmet`,
    {
      resolve: `gatsby-plugin-google-analytics`,
      options: {
        trackingId: "UA-155680388-1",
      }
    },
    `gatsby-plugin-sitemap`,
    {
      resolve: `gatsby-source-filesystem`,
      options: {
        name: `images`,
        path: `${__dirname}/src/images`,
      },
    },
    {
      resolve: `gatsby-source-filesystem`,
      options: {
        path: `${__dirname}/ebook`,
        name: "ebook",
      },
    },
    {
      resolve: `gatsby-source-filesystem`,
      options: {
        path: `${__dirname}/usage`,
        name: "usage",
      },
    },
    {
      resolve: "gatsby-transformer-remark",
      options: {
        plugins: [
          {
            resolve: `gatsby-remark-prismjs`,
            options: {
              classPrefix: "language-",
              aliases: {
                sh: "shell",
                es6: "javascript",
                env: "bash",
                mdx: "md",
                ".json": "json",
              },
            },
          },
          {
            resolve: `gatsby-remark-autolink-headers`,
            options: {
              offsetY: `80`,
            },
          },
        ],
      },
    },
    `gatsby-transformer-sharp`,
    `gatsby-plugin-sharp`,
    {
      resolve: `gatsby-plugin-manifest`,
      options: {
        name: `Another C Library`,
        short_name: `ac_`,
        start_url: `/`,
        icon: `src/images/ac-logo.png`,
      },
    },
    {
      resolve: `gatsby-plugin-typography`,
      options: {
        pathToConfigModule: `src/utils/typography`,
      },
    },
    `gatsby-plugin-offline`,
  ],
}
