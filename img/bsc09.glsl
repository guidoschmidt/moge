vec3 reflectionPass()
{
	// Variables
	vec3 shaded = vec3(0.0f, 0.0f, 0.0f);
	
	vec2 Fragment = getScreenSpacePosition();
	
	vec3 vsViewPosition = vec3(Camera.Projection * Camera.View * vec4(Camera.Position, 1.0f));
	vec3 vsNormal = vec3(texture(deferredNormalTex, Fragment));
	vec3 vsPosition = vec3(texture(deferredPositionTex, Fragment));
	vec3 vsViewVec = vsPosition - vsViewPosition;
	
	vec3 vsReflectionVec = reflect(normalize(vsViewVec), normalize(vsNormal));
	
	vec3 ssReflectionVec = viewSpaceToScreenSpace(vsReflectionVec);
	
	float sampledDepth = ssReflectionVec.z;
	float actualDepth = float(texture(deferredDepthTex, ssReflectionVec.xy));
	
	if(sampledDepth <= actualDepth)
	{
		shaded = vec3(texture(deferredDiffuseTex, ssReflectionVec.xy));
	}
	
	//shaded = vec3(ssReflectionVec.z);
	return shaded;
}