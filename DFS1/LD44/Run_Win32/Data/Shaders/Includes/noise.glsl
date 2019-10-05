vec2 random2( vec2 p ) {
    return fract(sin(vec2(dot(p,vec2(127.1,311.7)),dot(p,vec2(269.5,183.3))))*43758.5453);
}

float WorleyNoise(vec2 position, float scale, float time){

	position *= scale;

    // Tile the space
    vec2 i_position = floor(position);
    vec2 f_position = fract(position);

    float m_dist = 1.;  // minimun distance

    for (int y= -1; y <= 1; y++) {
        for (int x= -1; x <= 1; x++) {
            // Neighbor place in the grid
            vec2 neighbor = vec2(float(x),float(y));

            // Random position from current + neighbor place in the grid
            vec2 point = random2(i_position + neighbor);

			// Animate the point
            point = 0.5 + 0.5*sin(time + 6.2831*point);

			// Vector between the pixel and the point
            vec2 diff = neighbor + point - f_position;

            // Distance to the point
            float dist = length(diff);

            // Keep the closer distance
            m_dist = min(m_dist, dist);
        }
    }
	return m_dist;
}