import React, { useContext } from 'react';
import { graphql, StaticQuery, Link } from 'gatsby';
import { safelyGetSiteConfig } from '../cms/cms-utils';

import { MenuContext } from '../core/menu-context';

export const query = graphql`
	query {
		sitePage(path: { eq: "/config/" }) {
			context {
				frontmatter {
					menu_nav {
						text
						url
					}
				}
			}
		}
	}
`;

function Menu() {
	const { menu } = useContext(MenuContext);

	return (
		<StaticQuery
			query={query}
			render={(data) => {
				const menuArr = safelyGetSiteConfig(data.sitePage).menu_nav || [];

				return (
					<div className={menu ? 'Menu' : 'Menu-hide'}>
						<ul className="Menu-ul">
							{menuArr.map((item, i) => (
								<li className="Menu-li" key={i}>
									{item.url ? (
										<Link
											activeClassName="Menu-link-active"
											className="Menu-link"
											to={item.url}>
											{item.text}
										</Link>
									) : (
										item.text
									)}
								</li>
							))}
						</ul>
					</div>
				);
			}}
		/>
	);
}

export default Menu;
