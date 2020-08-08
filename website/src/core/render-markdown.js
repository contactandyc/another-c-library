import MDX from '@mdx-js/runtime';
import React from 'react';
import PropTypes from 'prop-types';

import CatchError from './catch-error';
import { CMS_COMPONENTS, CMS_SHORTCODES } from '../cms/cms-components';

/* Use this component to parse markdown using MDX. See MDX runtime for details.
 * The map provided to the components prop instructs MDX on how to render your HTML and
 * custom React components.
 *
 * @md: string - Markdown to be parsed
 */

function RenderMarkdown({ md }) {
	return (
		<CatchError>
			<MDX components={{ ...CMS_COMPONENTS, ...CMS_SHORTCODES }}>{md}</MDX>
		</CatchError>
	);
}

RenderMarkdown.defaultProps = {
	md: '',
};

RenderMarkdown.propTypes = {
	md: PropTypes.string.isRequired,
};

export default RenderMarkdown;
