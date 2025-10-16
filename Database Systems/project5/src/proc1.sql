-- ---------------------------------------------------------------------------------- --
-- ------------ Master procedure for Proces 1: Planning of an exhibition ------------ --
-- ---------------------------------------------------------------------------------- --

CREATE OR REPLACE PROCEDURE plan_exposition_with_zones_and_exemplars(
    p_name VARCHAR(50),
    p_description VARCHAR(500),
    p_expected_start_date TIMESTAMP,
    p_expected_end_date TIMESTAMP,
    p_zones VARCHAR[],
    p_exemplar_zone_pairs VARCHAR[]
)
AS $$
DECLARE
    v_exposition_id BIGINT;
    v_result INT;
    v_exemplar_id BIGINT;
    v_zone_id INT;
    v_exemplar_zone_pair VARCHAR;
BEGIN
    -- Plan the exposition with the specified zones
    CALL schedule_exposition(
        p_name,
        p_description,
        p_expected_start_date,
        p_expected_end_date,
        p_zones,
        v_result
    );

    IF v_result = 0 THEN
        RAISE NOTICE 'Exposition successfully scheduled.';
	ELSIF v_result = -1 THEN
        RAISE NOTICE 'Error: One or more specified zones do not exist.';
		RETURN;
    ELSIF v_result = -2 THEN
        RAISE NOTICE 'Error: One or more specified zones is already occupied within the given timeframe.';
		RETURN;
    ELSIF v_result = -3 THEN
        RAISE NOTICE 'Error: An unexpected error occurred during inserting the exposition.';
		CALL delete_garbage(p_name, p_description);
		RETURN;
    ELSIF v_result = -4 THEN
        RAISE NOTICE 'Error: An unexpected error occurred during assigning the zones.';
		CALL delete_garbage(p_name, p_description);
		RETURN;
    END IF;

    -- Get the ID of the scheduled exposition
    SELECT id INTO v_exposition_id
    FROM expositions
    WHERE name = p_name
    AND expected_start_date = p_expected_start_date
    AND expected_end_date = p_expected_end_date;

    -- Associate exemplars with the exposition
    FOREACH v_exemplar_zone_pair IN ARRAY p_exemplar_zone_pairs LOOP
        v_exemplar_id := split_part(v_exemplar_zone_pair, ',', 1)::BIGINT;
        v_zone_id := split_part(v_exemplar_zone_pair, ',', 2)::BIGINT;

        CALL insert_exemplar_into_exhibition(
            p_exemplar_id := v_exemplar_id,
            p_exposition_id := v_exposition_id,
            p_zone_id := v_zone_id,
            p_result := v_result
        );
		
		IF v_result = 0 THEN
        	RAISE NOTICE 'Exemplar added to exposition successfully.';
    	ELSIF v_result = -1 THEN
        	RAISE NOTICE 'Error: One or more provided exemplars do not exist.';
			CALL delete_garbage(p_name, p_description);
			RETURN;
    	ELSIF v_result = -2 THEN
        	RAISE NOTICE 'Error: The provided exposition-zone combination does not exist.';
			CALL delete_garbage(p_name, p_description);
			RETURN;
    	ELSIF v_result = -3 THEN
        	RAISE NOTICE 'Error: Exemplar exhibitied in another exposition with colliding time.';
			CALL delete_garbage(p_name, p_description);
			RETURN;
    	ELSIF v_result = -4 THEN
        	RAISE NOTICE 'Error: Failed to add exemplar to exposition.';
			CALL delete_garbage(p_name, p_description);
			RETURN;
		ELSIF v_result = -6 THEN
        	RAISE NOTICE 'Error: An unexpected error occurred during inserting the exemplar.';
			CALL delete_garbage(p_name, p_description);
			RETURN;
    	END IF;
	END LOOP;
	
    RAISE NOTICE 'Exposition planned successfully.';
END;
$$ LANGUAGE plpgsql;



-- ---------------------------------------------------------------------------------- --
-- ------------- Subprocedures for Proces 1: Planning of an exhibition -------------- --
-- ---------------------------------------------------------------------------------- --


CREATE OR REPLACE PROCEDURE schedule_exposition(  -- To plan an exposition
    -- Input params 
	p_name VARCHAR(50),
    p_description VARCHAR(500),
    p_expected_start_date TIMESTAMP,
    p_expected_end_date TIMESTAMP,
    p_zones VARCHAR[],
    OUT p_result INT
)
AS $$
DECLARE
    v_exposition_id BIGINT;
	v_existing_exposition_id BIGINT;
    v_zone_id INT;
	v_row_count INT;
	v_zone_name VARCHAR(50);
	v_existing_exposition INT;
BEGIN
    -- Initialize the return value
    p_result := 0;

    -- Check if the specified zones exist in the zones table
    FOREACH v_zone_name IN ARRAY p_zones LOOP
        SELECT id INTO v_zone_id 
		FROM zones 
		WHERE name = v_zone_name;
        IF NOT FOUND THEN
            -- If a specified zone does not exist, set the return value to -1 and return
            p_result := -1;
            RETURN;
        END IF;
    END LOOP;

    -- Check if the specified zones are free (no other exposition associated with them in the given time)
	FOREACH v_zone_name IN ARRAY p_zones LOOP
    	-- Translate the zone name into it's ID
		SELECT z.id INTO v_zone_id
		FROM zones z
		WHERE z.name = v_zone_name;
		
		SELECT 1
    	INTO v_existing_exposition
    	FROM expositions AS exp
    	JOIN exposition_exemplar_zone AS eez ON exp.id = eez.exposition_id
    	WHERE eez.zone_id = v_zone_id
    	AND (
        	(exp.real_start_date >= p_expected_start_date AND exp.real_start_date < p_expected_end_date)
        	OR (exp.real_end_date > p_expected_start_date AND exp.real_end_date <= p_expected_end_date)
        	OR (exp.real_start_date <= p_expected_start_date AND exp.real_end_date >= p_expected_end_date)
    	);
    	IF FOUND THEN
        	-- If the zone is not free, set the return value to -2 and return
        	p_result := -2;
        	RETURN;
    	END IF;
	END LOOP;

    -- Insert the exposition details into the expositions table
    BEGIN
        INSERT INTO expositions (name, description, expected_start_date, expected_end_date)
        VALUES (p_name, p_description, p_expected_start_date, p_expected_end_date)
        RETURNING id INTO v_exposition_id;
    EXCEPTION
        WHEN others THEN
            -- If an error occurs, set the return value to -3
            p_result := -3;
            RETURN;
    END;

    -- Insert the exposition and associated zones into the exposition_exemplar_zone table
    FOREACH v_zone_name IN ARRAY p_zones LOOP
        SELECT id INTO v_zone_id 
		FROM zones 
		WHERE name = v_zone_name;
        INSERT INTO exposition_exemplar_zone (exposition_id, zone_id)
        VALUES (v_exposition_id, v_zone_id);
    END LOOP;

    -- Check if any rows were affected by the insertion into exposition_exemplar_zone table
    GET DIAGNOSTICS v_row_count = ROW_COUNT;
    IF v_row_count = 0 THEN
        -- If no rows were inserted, set the return value to -4
        p_result := -4;
        RETURN;
    END IF;

    -- Set the return value to indicate success
    p_result := 0;
END;
$$ LANGUAGE plpgsql;




CREATE OR REPLACE PROCEDURE insert_exemplar_into_exhibition( -- To associate an exposition with an exemplar
    -- Input params 
    p_exemplar_id BIGINT,
    p_exposition_id BIGINT,
    p_zone_id INT,
    OUT p_result INT
)
AS $$
DECLARE
    v_row_count INT;
	v_new_exposition_start_date TIMESTAMP;
    v_new_exposition_end_date TIMESTAMP;
    v_existing_exposition INT;
BEGIN
    -- Initialize the return value
    p_result := 0;
	
	-- Check if the provided exemplar_id exists
    PERFORM 1
    FROM exemplars AS ex
    WHERE ex.id = p_exemplar_id;

    -- If the provided exemplar_id does not exist, set the return value to -1 and return
    IF NOT FOUND THEN
        p_result := -1;
        RETURN;
    END IF;

    -- Check if the provided exhibition_id and zone_id exist and are associated
    PERFORM 1
    FROM expositions AS e
    INNER JOIN exposition_exemplar_zone AS ee ON e.id = ee.exposition_id
    WHERE e.id = p_exposition_id AND ee.zone_id = p_zone_id;

    -- If the provided exhibition_id and zone_id are not associated, set the return value to -1 and return
    IF NOT FOUND THEN
        p_result := -2;
        RETURN;
    END IF;
    
    -- Check if the exemplar is not already exhibited in another exposition whose running time collides with the new one
    SELECT e.expected_start_date, e.expected_end_date
	INTO v_new_exposition_start_date, v_new_exposition_end_date
	FROM expositions AS e
	JOIN exposition_exemplar_zone AS eez ON e.id = eez.exposition_id
	WHERE eez.exemplar_id = p_exemplar_id;
	
	SELECT 1
	INTO v_existing_exposition
	FROM expositions AS exp
	JOIN exposition_exemplar_zone AS eex ON exp.id = eex.exposition_id
	WHERE eex.exemplar_id = p_exemplar_id
	AND (
    	(exp.real_start_date >= v_new_exposition_start_date AND exp.real_start_date < v_new_exposition_end_date)
    	OR (exp.real_end_date > v_new_exposition_start_date AND exp.real_end_date <= v_new_exposition_end_date)
    	OR (exp.real_start_date <= v_new_exposition_start_date AND exp.real_end_date >= v_new_exposition_end_date)
	);
	
	

    IF FOUND THEN
    -- If the exemplar is already exhibited in another exposition with colliding time, set the return value to -2 and return
        p_result := -3;
        RETURN;
    END IF;
    
    -- Insert the exemplar, exhibition, and zone association into the exposition_exemplar_zone table
    BEGIN
        INSERT INTO exposition_exemplar_zone (exemplar_id, exposition_id, zone_id)
        VALUES (p_exemplar_id, p_exposition_id, p_zone_id);

        -- Check if any rows were affected by the insertion
        GET DIAGNOSTICS v_row_count = ROW_COUNT;

        -- If no rows were inserted, set the return value to -3 and return
        IF v_row_count = 0 THEN
            p_result := -4;
            RETURN;
        END IF;

        -- Delete the obsolete row
        DELETE FROM exposition_exemplar_zone
        WHERE exposition_id = p_exposition_id AND exemplar_id IS NULL AND zone_id = p_zone_id;

        -- Check if any rows were affected by the deletion
        GET DIAGNOSTICS v_row_count = ROW_COUNT;

        -- If no rows were deleted, set the return value to -4 and return
        IF v_row_count = 0 THEN
            p_result := -5;
            RETURN;
        END IF;
    EXCEPTION
        WHEN others THEN
            -- If an error occurs, set the return value to -5
            p_result := -6;
            RETURN;
    END;
END;
$$ LANGUAGE plpgsql;



CREATE OR REPLACE PROCEDURE set_exposition_real_start_date(  -- To set the actual start date of the exposition
    p_exposition_id BIGINT,
    p_real_start_date TIMESTAMP
)
AS $$
DECLARE
	v_result INT;
BEGIN

    -- Check if the exposition with the provided ID exists
    PERFORM 1
    FROM expositions
    WHERE id = p_exposition_id;

    -- If the exposition does not exist, set the return value to -1 and return
    IF NOT FOUND THEN
        RAISE NOTICE 'Error: Exposition with the provided ID does not exist.';
        RETURN;
    END IF;

    -- Update the real_start_date of the exposition
    UPDATE expositions
    SET real_start_date = p_real_start_date
    WHERE id = p_exposition_id;

    -- Check if any rows were affected by the update
    GET DIAGNOSTICS v_result = ROW_COUNT;

    IF v_result = 1 THEN
        -- Succesfully updated the start date
		RAISE NOTICE 'Real start date of the exposition set successfully.';
		RETURN;
    ELSE
		-- Failed to update real_start_date
        RAISE NOTICE 'Error: Failed to update real start date of the exposition.';
		RETURN;
    END IF;
END;
$$ LANGUAGE plpgsql;



CREATE OR REPLACE PROCEDURE update_exhibited_exemplars() -- To update exemplar states periodically
AS $$
BEGIN
    -- Update exemplar states for expositions with real start date in the past
    UPDATE exemplars
    SET state = 'exhibited',
        storage_location = NULL
    WHERE id IN (
        SELECT eex.exemplar_id
        FROM expositions AS exp
        JOIN exposition_exemplar_zone AS eex ON exp.id = eex.exposition_id
        WHERE exp.real_start_date < CURRENT_TIMESTAMP
        AND exp.real_start_date IS NOT NULL
    );
END;
$$ LANGUAGE plpgsql;


CREATE OR REPLACE PROCEDURE delete_garbage(p_name VARCHAR, p_description VARCHAR) -- For deleting unwanted rows
AS $$
DECLARE
	v_exp_id BIGINT;
BEGIN
	SELECT exp.id INTO v_exp_id
	FROM expositions exp
	WHERE exp.name = p_name AND exp.description = p_description;
	
	DELETE FROM exposition_exemplar_zone
	WHERE exposition_id = v_exp_id;
	DELETE FROM expositions WHERE ID = v_exp_id;
END;
$$ LANGUAGE plpgsql;


-- ---------------------------------------------------------------------------------- --
-- ---------------------------------- Example calls --------------------------------- --
-- ---------------------------------------------------------------------------------- --

-- Example call for the whole planning procedure
CALL plan_exposition_with_zones_and_exemplars(
        'Exposition 1', -- p_name
        'Exposition Description 1', -- p_description
        '2024-04-16 10:00:00'::TIMESTAMP, -- p_expected_start_date
        '2024-05-16 17:00:00'::TIMESTAMP, -- p_expected_end_date
        ARRAY['Zone A', 'Zone B'], -- p_zones
        ARRAY[
            '1,1', -- exemplar id, zone id
            '2,2' -- exemplar id, zone id
        ] -- p_exemplar_zone_pairs
    );


-- Example call fro setting the real start date
CALL set_exposition_real_start_date(4, '2024-04-16 10:00:00'::TIMESTAMP)



-- Example call for inserting an exemplar
DO $$
DECLARE
    v_result INT;
BEGIN
    -- Call the procedure with sample values
    CALL insert_exemplar_into_exhibition(
        p_exemplar_id := 1,       -- Sample exemplar ID
        p_exposition_id := 10,    -- Sample exposition ID
        p_zone_id := 3,           -- Sample zone ID
        p_result := v_result      -- Output parameter for result
    );

    -- Check the result of the procedure call
    CASE v_result
        WHEN 0 THEN
            RAISE NOTICE 'Exemplar added to exhibition successfully.';
        WHEN -1 THEN
            RAISE NOTICE 'Error: Provided exemplar ID does not exist.';
        WHEN -2 THEN
            RAISE NOTICE 'Error: Provided exposition ID or zone ID is not associated.';
        WHEN -3 THEN
            RAISE NOTICE 'Error: Exemplar is already exhibited in another exposition with colliding time.';
        WHEN -4 THEN
            RAISE NOTICE 'Error: Failed to insert exemplar into exhibition.';
        WHEN -5 THEN
            RAISE NOTICE 'Error: Failed to delete obsolete row.';
        WHEN -6 THEN
            RAISE NOTICE 'Error: An unexpected error occurred.';
    END CASE;
END;
$$;









