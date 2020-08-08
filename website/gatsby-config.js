module.exports = {
	siteMetadata: {
		title: `Another C Library`,
		description: `Another C Library documentation website`,
		author: `Daniel Curtis @codescurtis`,
	},
	plugins: [
		`gatsby-plugin-dark-mode`,
		`@pauliescanlon/gatsby-mdx-embed`,
		`gatsby-plugin-react-helmet`,
		`gatsby-transformer-sharp`,
		`gatsby-plugin-sharp`,
		{
			resolve: `gatsby-source-filesystem`,
			options: {
				name: `images`,
				path: `${__dirname}/src/images`,
			},
		},
		{
			resolve: `gatsby-plugin-mdx`,
			options: {
				extensions: [`.mdx`, `.md`],
				defaultLayouts: {
					// This entry template will switch the page template based on
					// a frontmatter value provided in the CMS, allowing users to
					// choose different template layouts.
					default: require.resolve(`./src/templates/cms-entry.js`),
				},
			},
		},
		{
			resolve: `gatsby-plugin-manifest`,
			options: {
				name: `gatsby-mdx-netlify`,
				short_name: `starter`,
				start_url: `/`,
				background_color: `#663399`,
				theme_color: `#36B47B`,
				display: `minimal-ui`,
				icon: `src/images/gatsby-icon.png`, // This path is relative to the root of the site.
			},
		},
		{
			resolve: `gatsby-plugin-google-analytics`,
			options: {
				// The property ID; the tracking code won`t be generated without it
				trackingId: `YOUR_GOOGLE_ANALYTICS_TRACKING_ID`,
				// Defines where to place the tracking script - `true` in the head and `false` in the body
				head: false,
				// Avoids sending pageview hits from custom paths
				exclude: [`/admin/**`],
			},
		},
		{
			resolve: `gatsby-plugin-netlify-cms`,
			options: {
				modulePath: `${__dirname}/src/cms/cms.js`,
				customizeWebpackConfig: (config) => {
					config.node = {
						...config.node,
						fs: `empty`,
						child_process: `empty`,
						module: `empty`,
					};
				},
			},
		},
	],
};
