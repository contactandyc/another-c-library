// Disabling eslist because heading warning does not apply
/* eslint-disable */

import React from 'react';

import Heading from '../components/heading';
import SmartLink from '../components/smart-link';
import Repl from '../components/repl';
import NextPrev from '../components/next-prev';
import CodeBlock from '../components/code-block';

// Include all components that will be parsed by MDX as React components here.
// Any React component you'd like to allow your editors to use should be placed here.
const CMS_SHORTCODES = {
	Repl: (props) => <Repl {...props} />,
	NextPrev: (props) => <NextPrev {...props} />,
};

// Include any tags you'd like to replace with React components
const CMS_COMPONENTS = {
	h1: (props) => <Heading tag={1} {...props} />,
	h2: (props) => (
		<h2 {...props} id={props.children.replace(/ /g, '-').toLowerCase()} />
	),
	h3: (props) => <Heading tag={3} {...props} />,
	h4: (props) => <Heading tag={4} {...props} />,
	h5: (props) => <Heading tag={5} {...props} />,
	h6: (props) => <Heading tag={6} {...props} />,
	a: (props) => <SmartLink {...props} />,
	pre: (props) => <CodeBlock {...props} />,
};

export { CMS_SHORTCODES, CMS_COMPONENTS };
