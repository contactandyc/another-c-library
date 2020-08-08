import React, { Component } from 'react';

/*
 *  Used to help catch issues with invalid MDX in Netlify CMS
 *  Netlify CMS Previews have some trouble with hooks, so
 *  Use a class component here if possible
 */
class CatchError extends Component {
	constructor(props) {
		super(props);
		this.state = { hasError: false };
	}

	componentDidCatch(error, info) {
		// Display fallback UI
		this.setState({ hasError: true });
		// You can also log the error to an error reporting service
		console.warn(error, info);
	}

	render() {
		if (this.state.hasError) {
			return (
				<h2>
					Oops! Something went wrong. If you have edited this page recently,
					check your content and try again.
				</h2>
			);
		}

		return this.props.children;
	}
}

export default CatchError;
