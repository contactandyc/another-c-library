import React, { useContext } from 'react';
import { Link } from 'gatsby';

import Heading from '../components/heading';
import SEO from '../components/seo';
import RenderMarkdown from '../core/render-markdown';
import { safelyGetFrontMatter } from '../cms/cms-utils';
import { MenuContext } from '../core/menu-context';

function HomePageTemplate({ title, sections }) {
	const { toggleMenu } = useContext(MenuContext);

	return (
		<div className="HomePageTemplate">
			<SEO title={title} />

			<section className="HomePageTemplate-top">
				<Heading tag={1}>{title}</Heading>
				<div>
					<Link
						to="/tutorial/1-getting-started"
						onClick={() => toggleMenu(true)}>
						<button className="HomePageTemplate-btn">Get Started</button>
					</Link>
					<Link to="/docs/ac-allocator" onClick={() => toggleMenu(true)}>
						<button className="HomePageTemplate-btn">Documentation</button>
					</Link>
				</div>
			</section>

			<div className="HomePageTemplate-sections">
				<section className="HomePageTemplate-section">
					<Heading tag={2}>{sections[0].title}</Heading>
					<RenderMarkdown md={sections[0].body} />
				</section>
				<section className="HomePageTemplate-section">
					<Heading tag={2}>{sections[1].title}</Heading>
					<RenderMarkdown md={sections[1].body} />
				</section>
				<section className="HomePageTemplate-section">
					<Heading tag={2}>{sections[2].title}</Heading>
					<RenderMarkdown md={sections[2].body} />
				</section>
			</div>

			<section className="HomePageTemplate-landing">
				<div className="HomePageTemplate-title">
					<Heading tag={2}>{sections[3].title}</Heading>
					<p>{sections[3].description}</p>
					<Link to={sections[3].link}>
						<button
							className="HomePageTemplate-landing-btn"
							onClick={() => toggleMenu(true)}>
							{sections[3].button}
						</button>
					</Link>
				</div>
				<div className="HomePageTemplate-item">
					<RenderMarkdown md={sections[3].body} />
				</div>
			</section>

			<section className="HomePageTemplate-landing">
				<div className="HomePageTemplate-title">
					<Heading tag={2}>{sections[4].title}</Heading>
					<p>{sections[3].description}</p>
					<Link to={sections[4].link}>
						<button
							className="HomePageTemplate-landing-btn"
							onClick={() => toggleMenu(true)}>
							{sections[4].button}
						</button>
					</Link>
				</div>
				<div className="HomePageTemplate-item">
					<RenderMarkdown md={sections[4].body} />
				</div>
			</section>

			<section className="HomePageTemplate-landing">
				<div className="HomePageTemplate-title">
					<Heading tag={2}>{sections[5].title}</Heading>
					<p>{sections[5].description}</p>
					<Link to={sections[5].link}>
						<button
							className="HomePageTemplate-landing-btn"
							onClick={() => toggleMenu(true)}>
							{sections[5].button}
						</button>
					</Link>
				</div>
				<div className="HomePageTemplate-item">
					<RenderMarkdown md={sections[5].body} />
				</div>
			</section>

			<section className="HomePageTemplate-landing">
				<div className="HomePageTemplate-title">
					<Heading tag={2}>{sections[6].title}</Heading>
					<p>{sections[6].description}</p>
					<Link to={sections[6].link}>
						<button
							className="HomePageTemplate-landing-btn"
							onClick={() => toggleMenu(true)}>
							{sections[6].button}
						</button>
					</Link>
				</div>
				<div className="HomePageTemplate-item">
					<RenderMarkdown md={sections[6].body} />
				</div>
			</section>
		</div>
	);
}

function HomePage({ pageContext }) {
	return (
		<HomePageTemplate
			{...safelyGetFrontMatter(pageContext)}
			isPreview={false}
		/>
	);
}

export { HomePage, HomePageTemplate };
