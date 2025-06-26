using Godot;
using Godot.Collections;
using System.Linq;

[GlobalClass, Icon("res://Addons/SunshineClouds2/addons/SunshineClouds2/CloudsDriverIcon.svg")]
[Tool]
public partial class SunshineCloudsDriver : Node
{
	[Export] public bool UpdateContinuously { get { return _updateContinuously; } set { _updateContinuously = value; RetrieveTextureData(); } }
	private bool _updateContinuously = false;

	[ExportToolButton("Generate Clouds Resource", Icon = "Add")] public Callable generateaction => Callable.From(BuildNewClouds);

    [ExportSubgroup("Compositor Resource")]
    [Export] public SunshineClouds CloudsResource { get { return _cloudsResource; } set { _cloudsResource = value; RetrieveTextureData(); } }
    private SunshineClouds _cloudsResource;

    [ExportSubgroup("Optional World Environment")]
    [Export] public Environment AmbienceSampleEnvironment { get; set; }
	[Export] public bool UseEnvironmentAmbienceForFog { get; set; } = false;


    [ExportSubgroup("Light Controls")]
    [Export] public Array<DirectionalLight3D> TrackedDirectionalLights { get { return _trackedDirectionalLights; } set { _trackedDirectionalLights = value; RetrieveTextureData(); } }
    private Array<DirectionalLight3D> _trackedDirectionalLights = new Array<DirectionalLight3D>();
    [Export] public Array<int> TrackedDirectionalLightShadowSteps { get { return _trackedDirectionalLightShadowSteps; } set { _trackedDirectionalLightShadowSteps = value; RetrieveTextureData(); } }
    private Array<int> _trackedDirectionalLightShadowSteps = new Array<int>();
    [Export] public Array<OmniLight3D> TrackedPointLights { get { return _trackedPointLights; } set { _trackedPointLights = value; RetrieveTextureData(); } }
    private Array<OmniLight3D> _trackedPointLights = new Array<OmniLight3D>();

    [Export] public float DirectionalLightPowerMultiplier { get; set; } = 1.0f;
    [Export] public float PointLightPowerMultiplier { get; set; } = 1.0f;

    [ExportSubgroup("Wind Controls")]
    [Export] public Vector3 WindDirection { get; set; } = new Vector3(1.0f, 0.0f, 0.0f);
    [Export] public float ExtraLargeStructuresWindSpeed { get; set; } = 140.0f;
    [Export] public float LargeStructuresWindSpeed { get; set; } = 100.0f;
	[Export] public float MediumStructuresWindSpeed { get; set; } = 40.0f;
	[Export] public float SmallStructuresWindSpeed { get; set; } = 12.0f;


    [ExportGroup("Internal Use")]
    [Export] public Vector3 ExtraLargeCloudsPos = Vector3.Zero;
    [Export] public Vector3 LargeCloudsPos = Vector3.Zero;
	[Export] public Vector3 MediumCloudsPos = Vector3.Zero;
	[Export] public Vector3 SmallCloudsPos = Vector3.Zero;


    private float _extralargeCloudsDomain = 0.0f;
    private float _largeCloudsDomain = 0.0f;
	private float _mediumCloudsDomain = 0.0f;
	private float _smallCloudsDomain = 0.0f;

	private bool _updatingSettings = false;


    public override void _Ready()
	{
		if (_updateContinuously)
		{
			if (_cloudsResource == null)
			{
                _updateContinuously = false;
				return;
			}
			CallDeferred("RetrieveTextureData");
		}
	}


	public override void _Process(double delta)
	{
		
		if (_cloudsResource != null)
		{
            _cloudsResource.CurrentTime = Mathf.Wrap(_cloudsResource.CurrentTime + (float)delta * CloudsResource.DitherSpeed, 0.0f, _cloudsResource.DitherSpeed * 64.0f);
			if (_updateContinuously)
			{
				_updatingSettings = false;
                ExtraLargeCloudsPos += WindDirection * ExtraLargeStructuresWindSpeed * (float)delta;
                ExtraLargeCloudsPos = WrapVector(ExtraLargeCloudsPos, _extralargeCloudsDomain);
                LargeCloudsPos += WindDirection * LargeStructuresWindSpeed * (float)delta;
				LargeCloudsPos = WrapVector(LargeCloudsPos, _largeCloudsDomain);
				MediumCloudsPos += WindDirection * MediumStructuresWindSpeed * (float)delta;
				MediumCloudsPos = WrapVector(MediumCloudsPos, _mediumCloudsDomain);
				SmallCloudsPos += (-WindDirection + Vector3.Up).Normalized() * SmallStructuresWindSpeed * (float)delta;
				SmallCloudsPos = WrapVector(SmallCloudsPos, _smallCloudsDomain);

				_cloudsResource.ExtraLargeScaleCloudsPosition = ExtraLargeCloudsPos;
                _cloudsResource.LargeScaleCloudsPosition = LargeCloudsPos;
				_cloudsResource.MediumScaleCloudsPosition = MediumCloudsPos;
				_cloudsResource.DetailCloudsPosition = SmallCloudsPos;

				_cloudsResource.WindDirection = WindDirection;

                if (UseEnvironmentAmbienceForFog && AmbienceSampleEnvironment != null)
                {
                    _cloudsResource.AtmosphereColor = AmbienceSampleEnvironment.FogLightColor;
                    _cloudsResource.CloudAmbientColor = AmbienceSampleEnvironment.FogLightColor;
                }

				if (_trackedDirectionalLights.Count * 2.0 != _cloudsResource.DirectionalLightsData.Count || _trackedPointLights.Count * 2.0 != _cloudsResource.PointLightsData.Count) {
                    RetrieveTextureData();
                    return;
                }

                for (int i = 0; i < _trackedDirectionalLights.Count; i++)
                {
					if (_trackedDirectionalLights[i] == null)
					{
						continue;
					}
					if (DirectionLightDataChanged(_trackedDirectionalLights[i], _trackedDirectionalLightShadowSteps[i], _cloudsResource.DirectionalLightsData[i * 2], _cloudsResource.DirectionalLightsData[i * 2 + 1]))
					{
						RetrieveTextureData();
						return;
                    }
				}

                for (int i = 0; i < _trackedPointLights.Count; i++)
				{
                    if (_trackedPointLights[i] == null)
                    {
                        continue;
                    }
                    if (PointLightDataChanged(_trackedPointLights[i], _cloudsResource.PointLightsData[i * 2], _cloudsResource.PointLightsData[i * 2 + 1]))
                    {
                        RetrieveTextureData();
                        return;
                    }
                }
            }
		}
		else
		{
            _updateContinuously = false;
		}
	}

	private void BuildNewClouds()
	{
		if (IsInsideTree())
		{
			var env = RecursivelyFindEnv(GetTree().Root);
			if (env != null)
			{
				if (AmbienceSampleEnvironment == null)
				{
					AmbienceSampleEnvironment = env.Environment;
				}

				var newClouds = new SunshineClouds();
				ResourceSaver.Save(newClouds, "res://NewClouds.tres");
				CloudsResource = ResourceLoader.Load("res://NewClouds.tres") as SunshineClouds;

				if (env.Compositor == null) { 
					env.Compositor = new Compositor();
                }
				Array<CompositorEffect> newData = new();
				newData.Add(CloudsResource);
				foreach (var item in env.Compositor.CompositorEffects)
				{
                    newData.Add(item);
                }

                env.Compositor.CompositorEffects = newData;

				UpdateContinuously = true;
            }
			else
			{
				GD.PrintErr("No world environment found.");
            }
        }
    }

	private WorldEnvironment RecursivelyFindEnv(Node thisNode)
	{
		foreach (var child in thisNode.GetChildren())
		{
            if (child is WorldEnvironment env)
			{
				return env;
            }
			else
			{
				var result = RecursivelyFindEnv(child);
				if (result != null)
				{
					return result;
				}
            }
        }
		return null;
    }

    private void RetrieveTextureData()
	{
		if (_updatingSettings)
		{
			return;
		}
		_updatingSettings = true;

        if (_cloudsResource != null)
		{
			_extralargeCloudsDomain = _cloudsResource.ExtraLargeNoiseScale / 2.0f;
            _largeCloudsDomain = _cloudsResource.LargeNoiseScale / 2.0f;
			_mediumCloudsDomain = _cloudsResource.MediumNoiseScale / 2.0f;
			_smallCloudsDomain = _cloudsResource.SmallNoiseScale / 2.0f;

            _cloudsResource.DirectionalLightsData.Clear();
            _cloudsResource.PointLightsData.Clear();

            if (_trackedDirectionalLightShadowSteps.Count < _trackedDirectionalLights.Count)
            {
				while (_trackedDirectionalLightShadowSteps.Count < _trackedDirectionalLights.Count)
				{
                    _trackedDirectionalLightShadowSteps.Add(12);
                }
            }
			//float totalScale = _trackedDirectionalLights.Sum(x => x.LightEnergy);

			for (int i = 0; i < _trackedDirectionalLights.Count; i++)
			{
				if (_trackedDirectionalLights[i] != null)
				{
					DirectionalLight3D light = _trackedDirectionalLights[i];
					Vector3 lookDir = (light.GlobalTransform.Basis.Z).Normalized();
					_cloudsResource.DirectionalLightsData.Add(new Vector4(lookDir.X, lookDir.Y, lookDir.Z, (float)_trackedDirectionalLightShadowSteps[i]));
					_cloudsResource.DirectionalLightsData.Add(new Vector4(light.LightColor.R, light.LightColor.G, light.LightColor.B, light.LightColor.A * light.LightEnergy * DirectionalLightPowerMultiplier));
				}
            }
            //totalScale = _trackedPointLights.Sum(x => x.LightEnergy);
            for (int i = 0; i < _trackedPointLights.Count; i++)
            {
				if (_trackedPointLights[i] != null)
				{
					OmniLight3D light = _trackedPointLights[i];
					Vector3 lightPos = light.GlobalPosition;
					_cloudsResource.PointLightsData.Add(new Vector4(lightPos.X, lightPos.Y, lightPos.Z, light.OmniRange));
					_cloudsResource.PointLightsData.Add(new Vector4(light.LightColor.R, light.LightColor.G, light.LightColor.B, light.LightColor.A * light.LightEnergy * PointLightPowerMultiplier));
				}
            }

			_cloudsResource.LightsUpdated = true;
        }

		_updatingSettings = false;
    }

    private Vector3 WrapVector(Vector3 target, float domainSize)
	{
		if (target.X > domainSize)
		{
			target.X -= domainSize * 2.0f;
		}
		else if (target.X < -domainSize)
		{
			target.X += domainSize * 2.0f;
		}

		if (target.Y > domainSize)
		{
			target.Y -= domainSize * 2.0f;
		}
		else if (target.Y < -domainSize)
		{
			target.Y += domainSize * 2.0f;
		}

		if (target.Z > domainSize)
		{
			target.Z -= domainSize * 2.0f;
		}
		else if (target.Z < -domainSize)
		{
			target.Z += domainSize * 2.0f;
		}

		return target;
	}

	private bool DirectionLightDataChanged(DirectionalLight3D light, int shadowCount, Vector4 dirData, Vector4 colorData)
	{
		return 
			light.GlobalTransform.Basis.Z.X != dirData.X ||
            light.GlobalTransform.Basis.Z.Y != dirData.Y ||
            light.GlobalTransform.Basis.Z.Z != dirData.Z ||
            (float)shadowCount != dirData.W ||
            light.LightColor.R != colorData.X ||
            light.LightColor.G != colorData.Y ||
            light.LightColor.B != colorData.Z ||
            light.LightColor.A * light.LightEnergy != colorData.W;
    }

    private bool PointLightDataChanged(OmniLight3D light, Vector4 dirData, Vector4 colorData)
    {
        return
            light.GlobalPosition.X != dirData.X ||
            light.GlobalPosition.Y != dirData.Y ||
            light.GlobalPosition.Z != dirData.Z ||
            light.OmniRange != dirData.W ||
            light.LightColor.R != colorData.X ||
            light.LightColor.G != colorData.Y ||
            light.LightColor.B != colorData.Z ||
            light.LightColor.A * light.LightEnergy != colorData.W;
    }
}
